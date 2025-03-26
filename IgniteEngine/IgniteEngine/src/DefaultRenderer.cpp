#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer() :
	Renderer::Renderer()
{
	;
}

void DefaultRenderer::setDepthBuffer(DepthBuffer& depth_buffer) {
	_depth_buffer = &depth_buffer;
}

void DefaultRenderer::create() {
	configureQueues();
	createSemaphores();
}

void DefaultRenderer::destroy() {
	for (uint32_t i = 0; i < _nb_frame; i++) {
		vkDestroySemaphore(
			(*_graphics_queues)[0].getDevice()->getDevice(),
			_sem_render_starts[i],
			nullptr
		);
		vkDestroySemaphore(
			(*_graphics_queues)[0].getDevice()->getDevice(),
			_sem_render_ends[i],
			nullptr
		);
	}
}

void DefaultRenderer::render() {
	VkResult vk_result{};
	//uint32_t _available_img{ 0 };

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

	if (_is_not_first_frame) {
		_present_queues_in_flight[_present_frame].present(
			1,
			&_sem_render_ends[_present_frame],
			1,
			&_swapchain->getSwapchain(),
			&_available_img
		);
	}
	else {
		_is_not_first_frame = true;
	}
	_graphics_queues_in_flight[_present_frame].wait();
	
	_available_img = _swapchain->acquireNextImage(
		UINT64_MAX,
		_sem_render_starts[_current_frame],
		VK_NULL_HANDLE
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed acquiring next image!");
	}

	CommandBuffer cmd_buf = _graphics_queues_in_flight[_current_frame].newCommandBuffer();

	cmd_buf.reset();
	cmd_buf.begin();
	
	cmd_buf.dynamicRenderingPipelineBarrier(
		_swapchain->getCurrentImage(),
		*_depth_buffer
	);
	cmd_buf.beginRendering(
		_clear_color_value,
		_swapchain->getCurrentImage(),
		*_depth_buffer,
		_offset,
		_extent
	);

	const std::unordered_map<GraphicsPipeline*, Object3DArrays> gps_and_arrays = Object3D::getArrays(*this);

	for (const auto& gp_and_arrays : gps_and_arrays) {
		if (!gp_and_arrays.first) {
			continue;
		}
		const GraphicsPipeline& gp = *gp_and_arrays.first;
		cmd_buf.bindPipeline(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			gp.getPipeline()
		);
		std::vector<VkViewport> viewport{ gp.getViewport() };
		cmd_buf.setViewport(
			viewport
		);

		std::vector<VkRect2D> scissors{ gp.getScissors() };
		cmd_buf.setScissor(
			scissors
		);

		// Descriptor sets
		cmd_buf.bindDescriptorSets(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			gp.getPipelineLayout(),
			0,
			gp.getDescriptorSets().size(),
			gp.getDescriptorSets().data(),
			0,
			nullptr
		);
		
		const GraphicShader* gs = static_cast<const GraphicShader*>(&gp.getShader());
		const VkPushConstantRange& pc_range = gs->getPushConstantRange();
		cmd_buf.pushConstants(
			gp.getPipelineLayout(),
			pc_range.stageFlags,
			pc_range.offset,
			pc_range.size,
			gp.getPushConstants()
		);
		
		const VkDeviceSize buff_offset = { 0 };
		// Vertex Buffers
		const std::unordered_map<std::string, VkBuffer>& vertex_buffers = gp.getVertexBuffers();

		// Index Buffers
		const VkBuffer index_buffer = gp.getIndexBuffer();
		const IndexBufferInfo& index_buffer_info = gs->getIndexBufferInfo();

		// Index buffer binding
		cmd_buf.bindIndexBuffer(
			index_buffer,
			buff_offset,
			index_buffer_info.getIndexType()
		);

		// Vertex buffers binding
		for (const auto& vb : vertex_buffers) {
			const std::string& buf_name = vb.first;
			const VkBuffer vertex_buffer = vb.second;
			
			cmd_buf.bindVertexBuffer(
				gs->getVertexBufferDescription(buf_name).binding_desc.binding,
				1,
				&vertex_buffer,
				&buff_offset
			);
		}
		
		// Draw loop for mesh the number of instances
		GraphicsPipeline* no_const_gp = gp_and_arrays.first;
		uint32_t first_index = 0;
		for (const auto& m_o : gps_and_arrays.at(gp_and_arrays.first).mesh_objects) {
			Mesh* mesh = m_o.first;
			const std::vector<Object3D*>& objects = m_o.second;
			cmd_buf.drawIndexed(
				mesh->getIndicesNbElem(),
				objects.size(),
				first_index,
				0,
				0
			);
			first_index += mesh->getIndicesNbElem();
		}
	}
	
	cmd_buf.endRendering();
	cmd_buf.dynamicRenderingPipelineBarrierBack(_swapchain->getCurrentImage());
	cmd_buf.end();

	// Submit
	_graphics_queues_in_flight[_current_frame].submit(
		1,
		&_sem_render_starts[_current_frame],
		_pipeline_stage_flags.data(),
		1,
		&_sem_render_ends[_current_frame]
	);

	//_present_queues_in_flight[_current_frame].present(
	//	1,
	//	&_sem_render_ends[_current_frame],
	//	1,
	//	&_swapchain.getSwapchain(),
	//	&_available_img
	//);

	//_graphics_queues_in_flight[_current_frame].wait();

	_present_frame = _swapchain->getCurrentImageIndex();
	_current_frame = (_current_frame + 1) % _nb_frame;
}

Image& DefaultRenderer::getCurrentFrame() {
	return _swapchain->getImages()[_present_frame];
}

void DefaultRenderer::configureQueues() {
	for (uint32_t i = 0; i < _nb_frame; i++) {
		Queue gq = (*_graphics_queues)[0];
		_graphics_queues_in_flight.push_back(
			gq
		);

		Queue pq = (*_present_queues)[0];
		_present_queues_in_flight.push_back(
			pq
		);
	}
}

void DefaultRenderer::createSemaphores() {
	VkResult vk_result;
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	_sem_render_starts.resize(_nb_frame);
	_sem_render_ends.resize(_nb_frame);

	for (uint32_t i = 0; i < _nb_frame; i++) {
		vk_result = vkCreateSemaphore(
			(*_graphics_queues)[0].getDevice()->getDevice(),
			&semaphore_info,
			nullptr,
			&_sem_render_starts[i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}

		vk_result = vkCreateSemaphore(
			(*_graphics_queues)[0].getDevice()->getDevice(),
			&semaphore_info,
			nullptr,
			&_sem_render_ends[i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}
	}
}
