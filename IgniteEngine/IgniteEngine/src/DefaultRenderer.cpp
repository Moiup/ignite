#include "DefaultRenderer.h"

DefaultRenderer::DefaultRenderer() :
	Renderer::Renderer()
{
	;
}

void DefaultRenderer::create() {
	configureQueues();
	createSwapchain();
	createDepthBuffer();
	createSemaphores();
}

void DefaultRenderer::destroy() {
	_swapchain.destroy();

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
			&_swapchain.getSwapchain(),
			&_available_img
		);
	}
	else {
		_is_not_first_frame = true;
	}
	_graphics_queues_in_flight[_present_frame].wait();
	
	vk_result = vkAcquireNextImageKHR(
		(*_graphics_queues)[0].getDevice()->getDevice(),
		_swapchain.getSwapchain(),
		UINT64_MAX,
		_sem_render_starts[_current_frame],
		VK_NULL_HANDLE,
		&_available_img
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed acquiring next image!");
	}

	CommandBuffer cmd_buf = _graphics_queues_in_flight[_current_frame].newCommandBuffer();

	cmd_buf.reset();
	cmd_buf.begin();
	
	dynamicRenderingPipelineBarrier(cmd_buf);
	beginRendering(cmd_buf);

	const std::unordered_map<GraphicsPipeline*, Object3DArrays> gps_and_arrays = Object3D::getArrays(*this);

	for (const auto& gp_and_arrays : gps_and_arrays) {
		const GraphicsPipeline& gp = *gp_and_arrays.first;
		cmd_buf.bindPipeline(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			gp.getPipeline()
		);
		cmd_buf.setViewport(
			(std::vector<VkViewport>&)gp.getViewport()
		);
		cmd_buf.setScissor(
			(std::vector<VkRect2D>&)gp.getScissors()
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
				buff_offset
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
	dynamicRenderingPipelineBarrierBack(cmd_buf);
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

	_present_frame = _current_frame;
	_current_frame = (_current_frame + 1) % _nb_frame;
}

Image& DefaultRenderer::getCurrentFrame() {
	return _swapchain.getImages()[_present_frame];
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

void DefaultRenderer::createSwapchain() {
	PhysicalDevice* gpu = DefaultConf::gpu;
	std::vector<VkSurfaceFormatKHR> surface_formats = DefaultConf::render_window->getSurfaceFormats(*DefaultConf::gpu);
	std::vector<VkFormat> accepted_format = {
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_FORMAT_B8G8R8A8_SRGB
	};

	VkFormat found_format = {VK_FORMAT_UNDEFINED};
	VkColorSpaceKHR color_space{};

	for(const VkSurfaceFormatKHR& sf : surface_formats) {
		for(const VkFormat& af : accepted_format){
			if(sf.format == af) {
				found_format = sf.format;
				color_space = sf.colorSpace;
				break;
			}
		}

		if(found_format != VK_FORMAT_UNDEFINED){
			break;
		}
	}

	if(found_format == VK_FORMAT_UNDEFINED){
		throw std::runtime_error("Error: no proper format found!");
	}

	_swapchain.setDevice(
		_device
	);
	_swapchain.setSurface(
		_window->getSurface()
	);
	_swapchain.setImageFormat(found_format);
	_swapchain.setImageColorSpace(color_space);
	_swapchain.setMinImageCount(_nb_frame);
	_swapchain.setWidthHeight(
		_extent.width,
		_extent.height
	);
	_swapchain.setImageFormat(found_format);
	_swapchain.setQueueFamilyIndices(
		{(*_graphics_queues)[0].getFamilyIndex()}
	);
	_swapchain.create();

	for (Image& img : _swapchain.getImages()) {
		(*_graphics_queues)[0].changeLayout(
			img,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);
	}
	(*_graphics_queues)[0].submit();
	(*_graphics_queues)[0].wait();
}

void DefaultRenderer::createDepthBuffer() {
	_depth_buffer = DepthBuffer(
		_device,
		_window->getWidthInPixel(),
		_window->getHeightInPixel(),
		{ (*_graphics_queues)[0].getFamilyIndex() }
	);

	//_depth_buffer.setDevice(
	//	_device
	//);
	//_depth_buffer.setImageWidthHeight(
	//	_window->getWidthInPixel(),
	//	_window->getHeightInPixel()
	//);

	//_depth_buffer.setImageQueueFamilyIndices(
	//	{(*_graphics_queues)[0].getFamilyIndex()}
	//);
	//_depth_buffer.create();
}

void DefaultRenderer::dynamicRenderingPipelineBarrier(CommandBuffer& cmd_buf) {
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
	image_memory_barrier_frame.image = _swapchain.getImages()[_current_frame].getImage();
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

	cmd_buf.pipelineBarrier(
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier_frame
	);

	cmd_buf.pipelineBarrier(
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &depth_memory_barrier_frame
	);
}

void DefaultRenderer::dynamicRenderingPipelineBarrierBack(CommandBuffer& cmd_buf) {
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
	image_memory_barrier_frame_bk.image = _swapchain.getImages()[_available_img].getImage();
	image_memory_barrier_frame_bk.subresourceRange = subresource_range_frame_bk;

	cmd_buf.pipelineBarrier(
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier_frame_bk
	);
}

void DefaultRenderer::beginRendering(CommandBuffer& cmd_buf) {
	VkRenderingAttachmentInfoKHR color_attachment{};
	color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	color_attachment.imageView = _swapchain.getImages()[_available_img].getImageView();
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
	//rendering_info_khr.renderArea = graphics_pipeline.getScissors()[0];
	rendering_info_khr.renderArea.offset = _offset;
	rendering_info_khr.renderArea.extent = _extent;
	rendering_info_khr.layerCount = 1;
	rendering_info_khr.viewMask = 0;
	rendering_info_khr.colorAttachmentCount = 1;
	rendering_info_khr.pColorAttachments = &color_attachment;
	rendering_info_khr.pDepthAttachment = &depth_stencil_attachment;
	rendering_info_khr.pStencilAttachment = &depth_stencil_attachment;

	cmd_buf.beginRendering(
		rendering_info_khr
	);
}