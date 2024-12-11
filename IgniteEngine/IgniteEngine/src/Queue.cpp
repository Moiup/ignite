#include "Queue.h"

Queue::Queue()
{
	_command_pools = new std::list<CommandPool>{};
	_pending_command_buffers = new std::vector<VkCommandBuffer>;
	_shared_count = new int32_t(1);
}

Queue::Queue(const Queue& q)
{
	*this = q;
}

Queue::~Queue() {
	removeCommandPool();

	*_shared_count -= 1;
	if (!*_shared_count) {
		delete _shared_count;
	}

	if (!_command_pools) {
		delete _command_pools;
		delete _pending_command_buffers;
		_command_pools = nullptr;
		_pending_command_buffers = nullptr;
	}
}

Queue& Queue::operator=(const Queue& q) {
	_queue = q._queue;
	_device = q._device;
	_fence = q._fence;
	_family_index = q._family_index;
	_command_pool_indices = q._command_pool_indices;

	_command_pools = q._command_pools;
	_pending_command_buffers = q._pending_command_buffers;
	_shared_count = q._shared_count;
	*_shared_count += 1;

	removeCommandPool();
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
		0,
		&_queue
	);

	addCommandPool();
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
	CommandBuffer cmd_buf = _command_pool.newCommandBuffer();
	_pending_command_buffers->push_back(cmd_buf.getCommandBuffer());
	getNbPendingCommandBuffers() += 1;
	return cmd_buf;
}

void Queue::addCommandPool(VkFenceCreateFlags flags) {
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

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = flags;

	_fence = nullptr;
	VkResult vk_result = vkCreateFence(
		_device->getDevice(),
		&fence_info,
		nullptr,
		&_fence
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating fence!");
	}
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
	CommandBuffer cmd_buf = newCommandBuffer();
	
	cmd_buf.begin();

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
	cmd_buf.pushConstants(
		cp.getPipelineLayout(),
		cp.getShader()->getPushConstantRange().stageFlags,
		cp.getShader()->getPushConstantRange().offset,
		cp.getShader()->getPushConstantRange().size,
		cp.getPushConstants()
	);
	cmd_buf.dispatch(
		group_count_x,
		group_count_y,
		group_count_z
	);

	cmd_buf.end();
}

void Queue::flush() {
	VkSubmitInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = nullptr;
	info.waitSemaphoreCount = 0;
	info.pWaitSemaphores = nullptr;
	info.pWaitDstStageMask = nullptr;
	info.commandBufferCount = getNbPendingCommandBuffers();
	info.pCommandBuffers = getPendingCommandBufferStartPointer();
	info.signalSemaphoreCount = 0;
	info.pSignalSemaphores = nullptr;

	VkResult vk_result = vkQueueSubmit(
		_queue,
		1,
		&info,
		nullptr
	);
	if (vk_result != VK_SUCCESS) {
		std::cerr << "Error flushing!" << std::endl;
		throw "Error flushing!";
	}

	getStartIndexPendingCommandBuffers() += getNbPendingCommandBuffers();
	getNbPendingCommandBuffers() = 0;
}

const void Queue::submit(
	uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores
) {
	VkResult vk_result = vkResetFences(
		_device->getDevice(),
		1,
		&_fence
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: resetting fence failed!");
	}

	if (!getNbPendingCommandBuffers()) {
		return;
	}

	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		getNbPendingCommandBuffers(),
		getPendingCommandBufferStartPointer(),
		signalSemaphoreCount,
		pSignalSemaphores,
		_fence
	);

	getStartIndexPendingCommandBuffers() += getNbPendingCommandBuffers();
	getNbPendingCommandBuffers() = 0;
}

const void Queue::submitNoFence(
	uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores
) {
	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		getNbPendingCommandBuffers(),
		getPendingCommandBufferStartPointer(),
		signalSemaphoreCount,
		pSignalSemaphores,
		nullptr
	);

	getStartIndexPendingCommandBuffers() += getNbPendingCommandBuffers();
	getNbPendingCommandBuffers() = 0;
}

const void Queue::submit(
	uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t commandBufferCount,
	const VkCommandBuffer* pCommandBuffers,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	VkFence fence
) const {
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
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
}

const void Queue::wait() {
	if (getPendingCommandBuffers().empty()) {
		return;
	}

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

void Queue::createFence() {
	VkFenceCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
}

void Queue::copy(Image& src, Image& dst,
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
	image_copy.srcOffset = { 0 , 0 , 0 };
	image_copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.dstSubresource.mipLevel = 0;
	image_copy.dstSubresource.baseArrayLayer = 0;
	image_copy.dstSubresource.layerCount = 1;
	image_copy.dstOffset = { 0, 0, 0 };
	image_copy.extent.width = dst.getWidth();
	image_copy.extent.height = dst.getHeight();
	image_copy.extent.depth = 1;

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
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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
