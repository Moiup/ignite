#include "Queue.h"

Queue::Queue()
{
	_command_pools = new std::list<CommandPool>{};
	_pending_command_buffers = new std::vector<VkCommandBuffer>;
	_shared_count = new int32_t(1);
}

Queue::Queue(
	Device& device,
	uint32_t family_index,
	uint32_t queue_index
) :
	Queue::Queue()
{
	_device = &device;
	_family_index = family_index;
	_queue_index = queue_index;
	create();
}

Queue::Queue(const Queue& q)
{
	*this = q;
}

Queue::Queue(Queue&& q)
{
	*this = std::move(q);
}

Queue::~Queue() {
	destroy();
}

Queue& Queue::operator=(const Queue& q) {
	destroy();
	_queue = q._queue;
	_device = q._device;
	_fence = q._fence;
	_family_index = q._family_index;
	_queue_index = q._queue_index;
	_command_pool_indices = q._command_pool_indices;

	_command_pools = q._command_pools;
	_pending_command_buffers = q._pending_command_buffers;
	_shared_count = q._shared_count;
	*_shared_count += 1;

	addCommandPool();

	return *this;
}

Queue& Queue::operator=(Queue&& q) {
	destroy();
	_queue = std::move(q)._queue;
	q._queue = nullptr;
	_device = std::move(q)._device;
	q._device = nullptr;
	_fence = std::move(q)._fence;
	q._fence = nullptr;
	_family_index = std::move(q)._family_index;
	_queue_index = std::move(q)._queue_index;
	_command_pool_indices = std::move(q)._command_pool_indices;

	_command_pools = std::move(q)._command_pools;
	q._command_pools = nullptr;
	_pending_command_buffers = std::move(q)._pending_command_buffers;
	q._pending_command_buffers = nullptr;
	_shared_count = std::move(q)._shared_count;
	q._shared_count = nullptr;

	addCommandPool();

	return *this;
}

void Queue::setDevice(Device* device) {
	_device = device;
}

void Queue::setFamilyIndex(uint32_t family_index) {
	_family_index = family_index;
}

void Queue::create() {
	vkGetDeviceQueue(
		_device->getDevice(),
		_family_index,
		_queue_index,
		&_queue
	);

	addCommandPool();
	createFence();
}

const CommandPool& Queue::getCommandPool() const {
	return _command_pool;
}

CommandPool& Queue::getCommandPool() {
	return _command_pool;
}

VkQueue Queue::getQueue() {
	return _queue;
}

Device* Queue::getDevice() {
	return _device;
}

uint32_t Queue::getFamilyIndex() {
	return _family_index;
}

std::vector<VkCommandBuffer>& Queue::getPendingCommandBuffers() {
	return *_pending_command_buffers;
}

CommandBuffer& Queue::newCommandBuffer() {
	CommandBuffer& cmd_buf = _command_pool.newCommandBuffer();
	_pending_command_buffers->push_back(cmd_buf.getCommandBuffer());
	getNbPendingCommandBuffers() += 1;
	return cmd_buf;
}

void Queue::addCommandPool() {
	if (_command_pool.getPool()) {
		return;
	}

	CommandPool cmd_pool{};
	cmd_pool.setDevice(_device);
	cmd_pool.setQueueFamilyIndex(getFamilyIndex());
	cmd_pool.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	cmd_pool.create();

	_command_pool = cmd_pool;
	_command_pools->push_front(_command_pool);
}

void Queue::removeCommandPool() {
	if (_command_pool.getPool()) {
		std::list<CommandPool>::iterator it = _command_pools->begin();
		for (; it != _command_pools->end(); ++it) {
			if (*it == _command_pool) {
				_command_pools->erase(it);
				break;
			}
		}
	}
}

void Queue::destroy() {
	if (!_shared_count) {
		return;
	}

	removeCommandPool();

	*_shared_count -= 1;
	if (*_shared_count) {
		return;
	}
	delete _shared_count;

	if (_command_pools) {
		delete _command_pools;
		delete _pending_command_buffers;
		_command_pools = nullptr;
		_pending_command_buffers = nullptr;
	}

	if (_fence) {
		vkDestroyFence(
			_device->getDevice(),
			_fence,
			nullptr
		);
		_fence = nullptr;
	}
}

void Queue::changeLayout(
	Image& img,
	VkImageLayout new_layout,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	CommandBuffer cmd = newCommandBuffer();
	cmd.begin();
	changeLayout(
		cmd,
		img,
		new_layout,
		src_access_mask,
		dst_access_mask,
		src_stage_mask,
		dst_stage_mask
	);
	cmd.end();
}

void Queue::dispatch(
	ComputePipeline& cp,
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z
) {
	CommandBuffer& cmd_buf = newCommandBuffer();

	cmd_buf.begin();
	dispatch(
		cmd_buf,
		cp,
		group_count_x,
		group_count_y,
		group_count_z
	);
	cmd_buf.end();
}

void Queue::dispatch(
	CommandBuffer& cmd_buf,
	ComputePipeline& cp,
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z
) {

	cmd_buf.bindPipeline(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		cp.getPipeline()
	);
	cmd_buf.bindDescriptorSets(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		cp.getPipelineLayout(),
		0,
		cp.getDescriptorSets().size(),
		cp.getDescriptorSets().data(),
		0,
		nullptr
	);
	if (cp.getShader()->getPushConstantRange().size) {
		cmd_buf.pushConstants(
			cp.getPipelineLayout(),
			cp.getShader()->getPushConstantRange().stageFlags,
			cp.getShader()->getPushConstantRange().offset,
			cp.getShader()->getPushConstantRange().size,
			cp.getPushConstants()
		);
	}
	cmd_buf.dispatch(
		group_count_x,
		group_count_y,
		group_count_z
	);
}

void Queue::dispatchBarrier(
	ComputePipeline& cp,
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z,
	VkPipelineStageFlags2 src_stage_mask,
	VkPipelineStageFlags2 dst_stage_mask,
	VkAccessFlags2 src_access_mask,
	VkAccessFlags2 dst_access_mask
) {
	CommandBuffer& cmd_buf = newCommandBuffer();

	VkMemoryBarrier2 mem_bar{};
	mem_bar.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	mem_bar.pNext = nullptr;
	mem_bar.srcStageMask = src_stage_mask;
	mem_bar.dstStageMask = dst_stage_mask;
	mem_bar.srcAccessMask = src_access_mask;
	mem_bar.dstAccessMask = dst_access_mask;

	VkDependencyInfo dependency_info{};
	dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency_info.pNext = nullptr;
	dependency_info.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	dependency_info.memoryBarrierCount = 1;
	dependency_info.pMemoryBarriers = &mem_bar;
	dependency_info.bufferMemoryBarrierCount = 0;
	dependency_info.pBufferMemoryBarriers = nullptr;
	dependency_info.imageMemoryBarrierCount = 0;
	dependency_info.pImageMemoryBarriers = nullptr;

	cmd_buf.begin();
	dispatch(
		cmd_buf,
		cp,
		group_count_x,
		group_count_y,
		group_count_z
	);

	cmd_buf.pipelineBarrier(&dependency_info);

	cmd_buf.end();
}

void Queue::barrier(
	VkPipelineStageFlags2 src_stage_mask,
	VkPipelineStageFlags2 dst_stage_mask,
	VkAccessFlags2 src_access_mask,
	VkAccessFlags2 dst_access_mask
) {
	CommandBuffer& cmd_buf = newCommandBuffer();

	VkMemoryBarrier2 mem_bar{};
	mem_bar.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	mem_bar.pNext = nullptr;
	mem_bar.srcStageMask = src_stage_mask;
	mem_bar.dstStageMask = dst_stage_mask;
	mem_bar.srcAccessMask = src_access_mask;
	mem_bar.dstAccessMask = dst_access_mask;

	VkDependencyInfo dependency_info{};
	dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency_info.pNext = nullptr;
	dependency_info.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	dependency_info.memoryBarrierCount = 1;
	dependency_info.pMemoryBarriers = &mem_bar;
	dependency_info.bufferMemoryBarrierCount = 0;
	dependency_info.pBufferMemoryBarriers = nullptr;
	dependency_info.imageMemoryBarrierCount = 0;
	dependency_info.pImageMemoryBarriers = nullptr;

	cmd_buf.begin();
	cmd_buf.pipelineBarrier(&dependency_info);
	cmd_buf.end();
}

void Queue::beginRendering(
	glm::vec4& clear_color_value,
	Image& image,
	DepthBuffer& depth_buffer,
	VkOffset2D& offset,
	VkExtent2D& extent
) {
	CommandBuffer& cmd_buf = newCommandBuffer();

	cmd_buf.reset();
	cmd_buf.begin();

	cmd_buf.dynamicRenderingPipelineBarrier(
		image,
		depth_buffer
	);

	VkClearColorValue ccv = {
		clear_color_value.x,
		clear_color_value.y,
		clear_color_value.z
	};
	cmd_buf.beginRendering(
		ccv,
		image,
		depth_buffer,
		offset,
		extent
	);
}

void Queue::bindPipeline(GraphicsPipeline& gp) {
	CommandBuffer& cmd_buf = _command_pool.commandBuffers().back();

	cmd_buf.bindPipeline(
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		gp.getPipeline()
	);
	cmd_buf.setViewport(
		&gp.getViewport(),
		1
	);
	cmd_buf.setScissor(
		&gp.getScissors(),
		1
	);

	cmd_buf.bindDescriptorSets(
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		gp.getPipelineLayout(),
		0,
		gp.getDescriptorSets().size(),
		gp.getDescriptorSets().data(),
		0,
		nullptr
	);

	cmd_buf.pushConstants(
		gp.getPipelineLayout(),
		gp.getShader().getPushConstantRange().stageFlags,
		gp.getShader().getPushConstantRange().offset,
		gp.getShader().getPushConstantRange().size,
		gp.getPushConstants()
	);

	cmd_buf.bindIndexBuffer(
		gp.getIndexBuffer(),
		0,
		gp.getShader().getIndexBufferInfo().getIndexType()
	);

	VkDeviceSize v_offsets = { 0 };
	for (const auto& vb : gp.getVertexBuffers()) {
		const std::string& buf_name = vb.first;
		const VkBuffer vertex_buffer = vb.second;

		cmd_buf.bindVertexBuffer(
			gp.getShader().getVertexBufferDescription(buf_name).binding_desc.binding,
			1,
			&vertex_buffer,
			&v_offsets
		);
	}
}

void Queue::drawIndexed(
	uint32_t index_count,
	uint32_t instance_count,
	uint32_t first_index,
	uint32_t vertex_offset,
	uint32_t first_instance
) {
	CommandBuffer& cmd_buf = _command_pool.commandBuffers().back();

	cmd_buf.drawIndexed(
		index_count,
		instance_count,
		first_index,
		vertex_offset,
		first_instance
	);
}

void Queue::endRendering(Image &image) {
	CommandBuffer& cmd_buf = _command_pool.commandBuffers().back();
	VkImageLayout image_layout = image.getImageLayout();
	cmd_buf.endRendering();
	cmd_buf.dynamicRenderingPipelineBarrierBack(image);
	//changeLayout(cmd_buf, image, VK_IMAGE_LAYOUT_UNDEFINED);
	changeLayout(cmd_buf, image, image_layout);
	cmd_buf.end();
}

void Queue::flush(
	uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		getNbPendingCommandBuffers(),
		getPendingCommandBufferStartPointer(),
		signalSemaphoreCount,
		pSignalSemaphores,
		nullptr,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		getNbPendingCommandBuffers(),
		getPendingCommandBufferStartPointer(),
		signalSemaphoreCount,
		pSignalSemaphores,
		_fence,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t commandBufferCount,
	const VkCommandBuffer* pCommandBuffers,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	VkFence fence,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSumbintInfo
) {
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = timelineSemaphoreSumbintInfo;
	submit_info.waitSemaphoreCount = waitSemaphorecount;
	submit_info.pWaitSemaphores = pWaitSemaphores;
	submit_info.pWaitDstStageMask = pWaitDstStageMask;
	submit_info.commandBufferCount = commandBufferCount;
	submit_info.pCommandBuffers = pCommandBuffers;
	submit_info.signalSemaphoreCount = signalSemaphoreCount;
	submit_info.pSignalSemaphores = pSignalSemaphores;

	VkResult vk_result = vkQueueSubmit(_queue, 1, &submit_info, fence);

	if (vk_result != VK_SUCCESS) {
		std::cerr << "Queue submit failed: " << vk_result << std::endl;
		throw std::runtime_error("Error: failed submitting queue!");
	}

	getStartIndexPendingCommandBuffers() += getNbPendingCommandBuffers();
	getNbPendingCommandBuffers() = 0;
}

const void Queue::wait() {
	VkResult result = vkWaitForFences(
		_device->getDevice(),
		1, &_fence,
		VK_TRUE,
		UINT64_MAX
	);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Queue::submitSync: Error while waiting for the fence to finish.");
	}

	vkResetFences(
		_device->getDevice(),
		1,
		&_fence
	);

	getStartIndexPendingCommandBuffers() = 0;
	getNbPendingCommandBuffers() = 0;

	getPendingCommandBuffers().clear();
	for (CommandPool& cp : *_command_pools) {
		cp.reset();
	}
	//_command_pool.reset();
}

const void Queue::present(
	uint32_t waitSemaphoreCount,
	const VkSemaphore* pWaitSemaphores,
	uint32_t swapchainCount,
	const VkSwapchainKHR* pSwapchains,
	const uint32_t* pImageIndices,
	VkResult* pResults
) const {
	VkPresentInfoKHR present_info{};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pNext = nullptr;
	present_info.waitSemaphoreCount = waitSemaphoreCount;
	present_info.pWaitSemaphores = pWaitSemaphores;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = pSwapchains;
	present_info.pImageIndices = pImageIndices;
	present_info.pResults = nullptr;

	VkResult vk_result = vkQueuePresentKHR(
		_queue,
		&present_info
	);
	if (vk_result != VK_SUCCESS) {
		//std::cout << vk_result << std::endl;
		std::cerr << vk_result << std::endl;
		throw std::runtime_error("Error: failed presenting!");
	}
}

void Queue::waitIdle() {
	vkQueueWaitIdle(_queue);
}

void Queue::createFence(VkFenceCreateFlags flags) {
	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = flags;

	VkResult vk_result = vkCreateFence(
		_device->getDevice(),
		&fence_info,
		nullptr,
		&_fence
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating fence!");
	}

	vkResetFences(
		_device->getDevice(),
		1,
		&_fence
	);
}

//void Queue::copy(Image& src, Image& dst,
//	VkAccessFlags src_access_mask,
//	VkAccessFlags dst_access_mask,
//	VkPipelineStageFlags src_stage_mask,
//	VkPipelineStageFlags dst_stage_mask
//) {
//	copy(
//		src,
//		dst,
//		{ 0, 0, 0 },
//		{ 0, 0, 0 },
//		src_access_mask,
//		dst_access_mask,
//		src_stage_mask,
//		dst_stage_mask
//	);
//}

void Queue::copy(Image& src, Image& dst,
	VkOffset3D src_offset,
	VkOffset3D dst_offset,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	CommandBuffer cmd_buf = newCommandBuffer();
	cmd_buf.begin();

	VkImageLayout image_layout = dst.getImageLayout();

	VkImageLayout src_image_layout_initial = src.getImageLayout();
	changeLayout(
		cmd_buf,
		src,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_READ_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	changeLayout(
		cmd_buf,
		dst,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	// To do for each mip level
	// (To start, we consider only the original level -> 0)
	VkImageCopy image_copy{};
	image_copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.srcSubresource.mipLevel = 0;
	image_copy.srcSubresource.baseArrayLayer = 0;
	image_copy.srcSubresource.layerCount = 1;
	image_copy.srcOffset = src_offset;
	image_copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.dstSubresource.mipLevel = 0;
	image_copy.dstSubresource.baseArrayLayer = 0;
	image_copy.dstSubresource.layerCount = 1;
	image_copy.dstOffset = dst_offset;
	image_copy.extent.width = src.getWidth();
	image_copy.extent.height = src.getHeight();
	image_copy.extent.depth = src.getDepth();

	cmd_buf.copyImageToImage(
		src.getImage(),
		src.getImageLayout(),
		dst.getImage(),
		dst.getImageLayout(),
		1,
		&image_copy
	);

	changeLayout(
		cmd_buf,
		src,
		src_image_layout_initial,
		src.getStageAccessInfo().access_mask,
		dst_access_mask,
		src.getStageAccessInfo().stage_mask,
		dst_stage_mask
	);

	changeLayout(
		cmd_buf,
		dst,
		image_layout,
		dst.getStageAccessInfo().access_mask,
		dst_access_mask,
		dst.getStageAccessInfo().stage_mask,
		dst_stage_mask
	);

	cmd_buf.end();
}

void Queue::changeLayout(
	CommandBuffer& cmd_buf,
	Image& img,
	VkImageLayout new_layout,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	// Preparing the transfer with the image memory barrier
	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask = img.aspectMask();
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier{};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.image = img.getImage();
	image_memory_barrier.subresourceRange = subresource_range;
	image_memory_barrier.srcAccessMask = src_access_mask;
	image_memory_barrier.dstAccessMask = dst_access_mask;
	image_memory_barrier.oldLayout = img.getImageLayout();
	image_memory_barrier.newLayout = new_layout;

	cmd_buf.pipelineBarrier(
		src_stage_mask,
		dst_stage_mask,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier
	);

	img.getStageAccessInfo().access_mask = dst_access_mask;
	img.getStageAccessInfo().stage_mask = dst_stage_mask;

	img.setImageInitialLayout(new_layout);
}

VkCommandBuffer* Queue::getPendingCommandBufferStartPointer() {
	if(!getNbPendingCommandBuffers()){
		return nullptr;
	}
	return &((*_pending_command_buffers)[getStartIndexPendingCommandBuffers()]);
}

const uint32_t Queue::getStartIndexPendingCommandBuffers() const {
	return _command_pool_indices.start_i;
}

uint32_t& Queue::getStartIndexPendingCommandBuffers() {
	return _command_pool_indices.start_i;
}

const uint32_t Queue::getNbPendingCommandBuffers() const {
	return _command_pool_indices.nb_cmd_buf;
}

uint32_t& Queue::getNbPendingCommandBuffers() {
	return _command_pool_indices.nb_cmd_buf;
}
