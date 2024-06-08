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
	createGraphicsPipeline();
	createSemaphores();
}

void DefaultRenderer::destroy() {
	_swapchain.destroy();
	_depth_buffer.destroy();
	for (GraphicsPipeline& gp : _graphics_pipelines) {
		gp.destroy();
	}

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
	uint32_t available_img{ 0 };

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	
	_graphics_queues_in_flight[_current_frame].wait();

	vk_result = vkAcquireNextImageKHR(
		(*_graphics_queues)[0].getDevice()->getDevice(),
		_swapchain.getSwapchain(),
		UINT64_MAX,
		_sem_render_starts[_current_frame],
		VK_NULL_HANDLE,
		&available_img
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed acquiring next image!");
	}

	CommandBuffer cmd_buf = _graphics_queues_in_flight[_current_frame].allocateCommandBuffer();

	cmd_buf.reset();
	cmd_buf.begin();
	
	dynamicRenderingPipelineBarrier(cmd_buf);
	beginRendering(cmd_buf);

	for (GraphicsPipeline& gp : _graphics_pipelines) {
		cmd_buf.bindPipeline(
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			(VkPipeline&)gp.getPipeline()
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


		for (auto& pc_info : gp.getShader()->getPushConstantInfo()) {
			std::string name = pc_info.first;
			PushConstantInfo& info = pc_info.second;
			cmd_buf.pushConstants(
				gp.getPipelineLayout(),
				info.getStageFlags(),
				info.getOffset(),
				info.getSize(),
				gp.getShader()->getPushConstants()[name]
			);
		}
		
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
			cmd_buf.bindIndexBuffer(
				ib->getBuffer(),
				buff_offset[0],
				ib->getIndexType()
			);

			// Vertex buffers binding
			for (auto& vbs_pair : vbs_map) {
				std::string name = vbs_pair.first;
				std::vector<VertexBuffer*>& vertex_buffers = vbs_pair.second;
				VertexBuffer* vertex_buffer = vertex_buffers[i];
				cmd_buf.bindVertexBuffer(
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
	}
	
	cmd_buf.endRendering();
	dynamicRenderingPipelineBarrierBack(cmd_buf);
	cmd_buf.end();

	//VkCommandBuffer vk_cmd_buf = cmd_buf.getCommandBuffer();
	// Submit and present
	_graphics_queues_in_flight[_current_frame].submit(
		1,
		&_sem_render_starts[_current_frame],
		_pipeline_stage_flags.data(),
		1,
		&_sem_render_ends[_current_frame]
	);

	_present_queues_in_flight[_current_frame].present(
		1,
		&_sem_render_ends[_current_frame],
		1,
		&_swapchain.getSwapchain(),
		&available_img
	);

	if (DefaultConf::event->type == SDL_KEYDOWN) {
		if (DefaultConf::event->key.keysym.sym == SDLK_d) {
			Pixels pix{};
			pix.setPixels(_extent.width, _extent.height);
			pix.saveFile("../assets/ouioui.png");
		}
	}


	//// Copying depth image to a buffer
	//// Staging buffer
	//Buffer staging_buffer{};
	//staging_buffer.setLogicalDevice((VkDevice*)_logical_device->getDevice());
	//staging_buffer.setMemoryProperties(_gpu->getMemoryProperties());
	//staging_buffer.setFlags(0);
	//staging_buffer.setPNext(nullptr);
	//staging_buffer.setQueueFamilyIndexCount(0);
	//staging_buffer.setPQueueFamilyIndices(nullptr);
	//staging_buffer.setSize(_window->getHeight() * _window->getWidth() * 1 * sizeof(uint64_t));
	//staging_buffer.setUsage(VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	//staging_buffer.setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	//staging_buffer.create();

	//staging_buffer.allocateMemory(
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	//);
	//staging_buffer.bind();

	//std::vector<VkBufferImageCopy> buffer_image_copy_arr{};

	//// To do for each mip level
	//// (To start, we consider only the original level -> 0)
	//VkBufferImageCopy buffer_image_copy{};
	//buffer_image_copy.bufferOffset = 0;
	//buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	//buffer_image_copy.imageSubresource.mipLevel = 0;
	//buffer_image_copy.imageSubresource.baseArrayLayer = 0;
	//buffer_image_copy.imageSubresource.layerCount = 1;
	//buffer_image_copy.imageExtent.width = _window->getWidth();
	//buffer_image_copy.imageExtent.height = _window->getHeight();
	//buffer_image_copy.imageExtent.depth = 1;
	//buffer_image_copy_arr.push_back(buffer_image_copy);

	//// Preparing the transfer with the image memory barrier
	//CommandBuffer copy_cmd{};
	//copy_cmd.setLogicalDevice((VkDevice*)_logical_device->getDevice());
	//copy_cmd.setCommandPool((VkCommandPool*)&_command_pool->getPool());
	//copy_cmd.setLevel(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	//copy_cmd.create();
	//copy_cmd.begin();

	//VkBufferMemoryBarrier buffer_memory_barrier{};
	//buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	//buffer_memory_barrier.pNext = nullptr;
	//buffer_memory_barrier.srcAccessMask = 0;
	//buffer_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	//buffer_memory_barrier.buffer = staging_buffer.getBuffer();
	//buffer_memory_barrier.offset = 0;
	//buffer_memory_barrier.size = _window->getHeight() * _window->getWidth() * 1 * sizeof(char);

	//VkImageSubresourceRange depth_subresource_range_frame{};
	//depth_subresource_range_frame.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	//depth_subresource_range_frame.baseMipLevel = 0;
	//depth_subresource_range_frame.levelCount = 1;
	//depth_subresource_range_frame.baseArrayLayer = 0;
	//depth_subresource_range_frame.layerCount = 1;

	//VkImageMemoryBarrier depth_memory_barrier_frame{};
	//depth_memory_barrier_frame.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//depth_memory_barrier_frame.pNext = nullptr;
	//depth_memory_barrier_frame.srcAccessMask = 0;
	//depth_memory_barrier_frame.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	//depth_memory_barrier_frame.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	//depth_memory_barrier_frame.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	//depth_memory_barrier_frame.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//depth_memory_barrier_frame.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//depth_memory_barrier_frame.image = _depth_buffer.getImage();
	//depth_memory_barrier_frame.subresourceRange = depth_subresource_range_frame;

	//copy_cmd.pipelineBarrier(
	//	VK_PIPELINE_STAGE_HOST_BIT,
	//	VK_PIPELINE_STAGE_TRANSFER_BIT,
	//	0,
	//	0, nullptr,
	//	1, &buffer_memory_barrier,
	//	1, &depth_memory_barrier_frame
	//);

	//depth_memory_barrier_frame.srcAccessMask = 0;
	//depth_memory_barrier_frame.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//depth_memory_barrier_frame.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//depth_memory_barrier_frame.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//vkCmdCopyImageToBuffer(
	//	*copy_cmd.getCommandBuffer(),
	//	_depth_buffer.getImage(),
	//	VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
	//	staging_buffer.getBuffer(),
	//	buffer_image_copy_arr.size(),
	//	buffer_image_copy_arr.data()
	//);

	//copy_cmd.pipelineBarrier(
	//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
	//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
	//	0,
	//	0, nullptr,
	//	0, nullptr,
	//	1, &depth_memory_barrier_frame
	//);

	//copy_cmd.end();
	//copy_cmd.flush(_logical_device->getDefaultQueue());
	//copy_cmd.free();

	//float* val = (float*)staging_buffer.getValues();

	//std::vector<glm::vec4> depth;
	//depth.resize(_window->getWidth() * _window->getHeight());

	//for (uint32_t i = 0; i < _window->getWidth() * _window->getHeight(); i++) {
	//	float f = val[i] * 255;
	//	depth[i].r = f;
	//	depth[i].g = f;
	//	depth[i].b = f;
	//	depth[i].a = 255;
	//}

	//Texture tex;
	//tex.setPixels(depth, _window->getWidth(), _window->getHeight());
	//tex.writeFile("../assets/test_test.bmp");

	//delete val;

	//staging_buffer.destroy();

	_current_frame = (_current_frame + 1) % _nb_frame;
}

void DefaultRenderer::configureQueues() {
	for (uint32_t i = 0; i < _nb_frame; i++) {
		Queue gq = (*_graphics_queues)[0];
		gq.addCommandPool();
		_graphics_queues_in_flight.push_back(
			gq
		);

		Queue pq = (*_present_queues)[0];
		pq.addCommandPool();
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

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

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
	_swapchain.setQueue(
		&(*_graphics_queues)[0]
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
		{(*_graphics_queues)[0].getFamilyIndex()}
	);
	_swapchain.create();
}

void DefaultRenderer::createDepthBuffer() {
	_depth_buffer.setQueue(
		&(*_graphics_queues)[0]
	);
	_depth_buffer.setImageWidthHeight(
		_window->getWidthInPixel(),
		_window->getHeightInPixel()
	);

	_depth_buffer.setImageQueueFamilyIndices(
		{(*_graphics_queues)[0].getFamilyIndex()}
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
		gp.setShader(shader);
		gp.setNbFrame(_nb_frame);
		gp.setSwapchain(&_swapchain);
		gp.setDepthBuffer(&_depth_buffer);
		//gp.setCullMode(VK_CULL_MODE_BACK_BIT);
		gp.create();
		_graphics_pipelines.push_back(gp);
	}
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
	image_memory_barrier_frame_bk.image = _swapchain.getImages()[_current_frame].getImage();
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
	color_attachment.imageView = _swapchain.getImages()[_current_frame].getImageView();
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