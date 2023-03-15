#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer() :
	Renderer::Renderer()
{
	;
}

void DefaultRenderer::create() {
	createCommandBuffers();
	createSwapchain();
	createDepthBuffer();
	createGraphicsPipeline();
	createFencesAndSemaphores();
}

void DefaultRenderer::destroy() {
	_swapchain.destroy();
	_depth_buffer.destroy();
	for (GraphicsPipeline& gp : _graphics_pipelines) {
		gp.destroy();
	}

	for (auto& cmd_buffer : _command_buffers) {
		cmd_buffer.free();
	}
	for (uint32_t i = 0; i < _nb_frame; i++) {
		vkDestroySemaphore(
			*_logical_device->getDevice(),
			_sem_render_starts[i],
			nullptr
		);
		vkDestroySemaphore(
			*_logical_device->getDevice(),
			_sem_render_ends[i],
			nullptr
		);

		vkDestroyFence(
			*_logical_device->getDevice(),
			_fences[i],
			nullptr
		);
	}
}

void DefaultRenderer::render() {
	VkResult vk_result{};
	uint32_t available_img{ 0 };

	vkWaitForFences(
		*_logical_device->getDevice(),
		1,
		&_fences[_current_frame],
		VK_TRUE,
		UINT64_MAX
	);
	vkResetFences(
		*_logical_device->getDevice(),
		1,
		&_fences[_current_frame]
	);

	vk_result = vkAcquireNextImageKHR(
		*_logical_device->getDevice(),
		_swapchain.getSwapchain(),
		UINT64_MAX,
		_sem_render_starts[_current_frame],
		VK_NULL_HANDLE,
		&available_img
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed acquiring next image!");
	}

	_command_buffers[_current_frame].reset();
	_command_buffers[_current_frame].begin();
	dynamicRenderingPipelineBarrier();

	for (GraphicsPipeline& gp : _graphics_pipelines) {
		beginRendering(gp);
		_command_buffers[_current_frame].bindPipeline(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			(VkPipeline&)gp.getPipeline()
		);
		_command_buffers[_current_frame].setViewport(
			(std::vector<VkViewport>&)gp.getViewport()
		);
		_command_buffers[_current_frame].setScissor(
			(std::vector<VkRect2D>&)gp.getScissors()
		);

		// Descriptor sets
		_command_buffers[_current_frame].bindDescriptorSets(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			gp.getPipelineLayout(),
			0,
			gp.getDescriptorSets().size(),
			gp.getDescriptorSets().data(),
			0,
			nullptr
		);

		const VkDeviceSize buff_offset[1] = { 0 };
		GraphicShader* gs = gp.getShader();
		// Vertex Buffers
		std::unordered_map<std::string, std::vector<VertexBuffer* >>& vbs_map = gs->getVertexBuffers();
		std::vector<VertexBuffer*>& vbs = vbs_map.begin()->second;
		// Index Buffers
		std::unordered_map<std::string, std::vector<IndexBuffer*>>& ibs_map = gs->getIndexBuffers();
		std::vector<IndexBuffer*>& ibs = ibs_map.begin()->second;
		uint32_t nb_buff = vbs.size();
		for (uint32_t i = 0; i < nb_buff; i++) {
			// Index buffer binding
			IndexBuffer* ib = ibs[i];
			_command_buffers[_current_frame].bindIndexBuffer(
				ib->getBuffer(),
				buff_offset[0],
				ib->getIndexType()
			);

			// Vertex buffers binding
			for (auto& vbs_pair : vbs_map) {
				std::string name = vbs_pair.first;
				std::vector<VertexBuffer*>& vertex_buffers = vbs_pair.second;
				VertexBuffer* vertex_buffer = vertex_buffers[i];
				_command_buffers[_current_frame].bindVertexBuffer(
					gs->getVertexBufferInfo(name).getFirstBinding(),
					1,
					&vertex_buffer->getBuffer(),
					buff_offset
				);
			}

			// Draw loop for mesh the number of instances
			uint32_t first_index = 0;
			for (auto& m_o : Object3D::getMeshObjects(this, gp.getShader())) {
				Mesh* mesh = m_o.first;
				std::vector<Object3D*>& objects = m_o.second;
				_command_buffers[_current_frame].drawIndexed(
					mesh->getIndicesNbElem(),
					objects.size(),
					first_index,
					0,
					0
				);
				first_index += mesh->getIndicesNbElem();
			}
		}

		_command_buffers[_current_frame].endRendering();
	}

	dynamicRenderingPipelineBarrierBack();
	_command_buffers[_current_frame].end();

	// Submit and present
	_logical_device->getQueue("graphic_queue")->submit(
		1,
		&_sem_render_starts[_current_frame],
		_pipeline_stage_flags.data(),
		1,
		_command_buffers[_current_frame].getCommandBuffer(),
		1,
		&_sem_render_ends[_current_frame],
		&_fences[_current_frame]
	);
	_logical_device->getQueue("present_queue")->present(
		1,
		&_sem_render_ends[_current_frame],
		1,
		&_swapchain.getSwapchain(),
		&available_img,
		nullptr
	);

	_current_frame = (_current_frame + 1) % _nb_frame;
}

void DefaultRenderer::createFencesAndSemaphores() {
	VkResult vk_result;
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	_sem_render_starts.resize(_nb_frame);
	_sem_render_ends.resize(_nb_frame);
	_fences.resize(_nb_frame);

	for (uint32_t i = 0; i < _nb_frame; i++) {
		vk_result = vkCreateSemaphore(
			*_logical_device->getDevice(),
			&semaphore_info,
			nullptr,
			&_sem_render_starts[i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}

		vk_result = vkCreateSemaphore(
			*_logical_device->getDevice(),
			&semaphore_info,
			nullptr,
			&_sem_render_ends[i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}

		vk_result = vkCreateFence(
			*_logical_device->getDevice(),
			&fence_info,
			nullptr,
			&_fences[i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating fence!");
		}
	}
}

void DefaultRenderer::createSwapchain() {
	_swapchain.setLogicalDevice(
		(VkDevice*)_logical_device->getDevice()
	);
	_swapchain.setSurface(
		_window->getSurface()
	);
	_swapchain.setMinImageCount(_nb_frame);
	_swapchain.setWidthHeight(
		_window->getWidthInPixel(),
		_window->getHeightInPixel()
	);
	_swapchain.setQueueFamilyIndices(
		_logical_device->getQueueFamilyIndexes()
	);
	_swapchain.create();
}

void DefaultRenderer::createDepthBuffer() {
	_depth_buffer.setLogicalDevice(
		(VkDevice*)_logical_device->getDevice()
	);
	_depth_buffer.setImageWidthHeight(
		_window->getWidthInPixel(),
		_window->getHeightInPixel()
	);
	_depth_buffer.setImageQueueFamilyIndices(
		_logical_device->getQueueFamilyIndexes()
	);
	_depth_buffer.setMemoryProperties(
		_gpu->getMemoryProperties()
	);
	_depth_buffer.create();
}

void DefaultRenderer::createGraphicsPipeline() {
	auto& mesh_objects = Object3D::getMeshObjects(this);
	
	for (auto& mo: mesh_objects) {
		GraphicShader* shader = mo.first;
		if (!shader) {
			continue;
		}
		GraphicsPipeline gp{};
		gp.setLogicalDevice(
			(VkDevice*)_logical_device->getDevice()
		);
		gp.setPhysicalDevice(_gpu);
		gp.setNbFrame(_nb_frame);
		gp.setSwapchain(&_swapchain);
		gp.setDepthBuffer(&_depth_buffer);
		gp.setShader(shader);
		gp.create();
		_graphics_pipelines.push_back(gp);
	}
}

void DefaultRenderer::createCommandBuffers() {
	for (uint32_t i = 0; i < _nb_frame; i++) {
		_command_buffers.push_back(
			_command_pool->createCommandBuffer()
		);
	}
}

void DefaultRenderer::dynamicRenderingPipelineBarrier() {
	VkImageSubresourceRange subresource_range_frame{};
	subresource_range_frame.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range_frame.baseMipLevel = 0;
	subresource_range_frame.levelCount = 1;
	subresource_range_frame.baseArrayLayer = 0;
	subresource_range_frame.layerCount = 1;

	VkImageSubresourceRange depth_subresource_range_frame{};
	depth_subresource_range_frame.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depth_subresource_range_frame.baseMipLevel = 0;
	depth_subresource_range_frame.levelCount = 1;
	depth_subresource_range_frame.baseArrayLayer = 0;
	depth_subresource_range_frame.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier_frame{};
	image_memory_barrier_frame.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier_frame.pNext = nullptr;
	image_memory_barrier_frame.srcAccessMask = 0;
	image_memory_barrier_frame.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	image_memory_barrier_frame.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_memory_barrier_frame.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	image_memory_barrier_frame.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame.image = _swapchain.getImages()[_current_frame];
	image_memory_barrier_frame.subresourceRange = subresource_range_frame;

	VkImageMemoryBarrier depth_memory_barrier_frame{};
	depth_memory_barrier_frame.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	depth_memory_barrier_frame.pNext = nullptr;
	depth_memory_barrier_frame.srcAccessMask = 0;
	depth_memory_barrier_frame.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	depth_memory_barrier_frame.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_memory_barrier_frame.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_memory_barrier_frame.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depth_memory_barrier_frame.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depth_memory_barrier_frame.image = _depth_buffer.getImage();
	depth_memory_barrier_frame.subresourceRange = depth_subresource_range_frame;

	_command_buffers[_current_frame].pipelineBarrier(
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier_frame
	);

	_command_buffers[_current_frame].pipelineBarrier(
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &depth_memory_barrier_frame
	);
}

void DefaultRenderer::dynamicRenderingPipelineBarrierBack() {
	// Changing image layout
	VkImageSubresourceRange subresource_range_frame_bk{};
	subresource_range_frame_bk.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range_frame_bk.baseMipLevel = 0;
	subresource_range_frame_bk.levelCount = 1;
	subresource_range_frame_bk.baseArrayLayer = 0;
	subresource_range_frame_bk.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier_frame_bk{};
	image_memory_barrier_frame_bk.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier_frame_bk.pNext = nullptr;
	image_memory_barrier_frame_bk.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	image_memory_barrier_frame_bk.dstAccessMask = 0;
	image_memory_barrier_frame_bk.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	image_memory_barrier_frame_bk.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	image_memory_barrier_frame_bk.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame_bk.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame_bk.image = _swapchain.getImages()[_current_frame];
	image_memory_barrier_frame_bk.subresourceRange = subresource_range_frame_bk;

	_command_buffers[_current_frame].pipelineBarrier(
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier_frame_bk
	);
}

void DefaultRenderer::beginRendering(GraphicsPipeline& graphics_pipeline) {
	VkRenderingAttachmentInfoKHR color_attachment{};
	color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	color_attachment.imageView = _swapchain.getImageViews()[_current_frame];
	color_attachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.clearValue.color = _clear_color_value;

	VkRenderingAttachmentInfoKHR depth_stencil_attachment{};
	depth_stencil_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	depth_stencil_attachment.imageView = _depth_buffer.getImageView();
	depth_stencil_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_stencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_stencil_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_stencil_attachment.clearValue.color.float32[0] = 0.0f;
	depth_stencil_attachment.clearValue.color.float32[1] = 0.0f;
	depth_stencil_attachment.clearValue.color.float32[2] = 0.0f;
	depth_stencil_attachment.clearValue.color.float32[3] = 0.0f;
	depth_stencil_attachment.clearValue.depthStencil.depth = 1.0f;
	depth_stencil_attachment.clearValue.depthStencil.stencil = 0;

	VkRenderingInfoKHR rendering_info_khr{};
	rendering_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	rendering_info_khr.pNext = nullptr;
	rendering_info_khr.flags = 0;
	rendering_info_khr.renderArea = graphics_pipeline.getScissors()[0];
	rendering_info_khr.layerCount = 1;
	rendering_info_khr.viewMask = 0;
	rendering_info_khr.colorAttachmentCount = 1;
	rendering_info_khr.pColorAttachments = &color_attachment;
	rendering_info_khr.pDepthAttachment = &depth_stencil_attachment;
	rendering_info_khr.pStencilAttachment = &depth_stencil_attachment;

	_command_buffers[_current_frame].beginRendering(
		rendering_info_khr
	);
}