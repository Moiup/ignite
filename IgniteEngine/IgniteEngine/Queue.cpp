#include "Queue.h"

std::unordered_map<VkQueue, std::vector<CommandPool>> Queue::_cmd_pools;
std::unordered_map<CommandPool*, std::vector<VkCommandBuffer>> Queue::_pending_command_buffers;
std::unordered_map<CommandPool*, CommandPoolSubmitBuffersIndices> Queue::_command_pool_indices;

Queue::Queue()
{

}

Queue::Queue(Queue& q) {
	*this = q;
}


Queue::Queue(const Queue& q) {
	*this = q;
}

//
//Queue::Queue(Queue&& q) {
//	*this = q;
//}

Queue& Queue::operator=(const Queue& q) {
	_queue = q._queue;
	_device = q._device;
	_fence = q._fence;
	_cmd_pool_i = q._cmd_pool_i;
	_family_index = q._family_index;

	return *this;
}

void Queue::setDevice(Device* device) {
	_device = device;
}

void Queue::setFamilyIndex(uint32_t family_index) {
	_family_index = family_index;
}

void Queue::setGPU(PhysicalDevice* gpu) {
	_gpu = gpu;
}

void Queue::create() {
	vkGetDeviceQueue(
		_device->getDevice(),
		_family_index,
		0,
		&_queue
	);

	addCommandPool();

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = 0;

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

CommandPool& Queue::getCommandPool() const {
	return _cmd_pools[_queue][_cmd_pool_i];
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

PhysicalDevice* Queue::getGPU() {
	return _gpu;
}

std::vector<VkCommandBuffer>& Queue::getPendingCommandBuffers() {
	return _pending_command_buffers[&getCommandPool()];
}

void Queue::addCommandPool() {
	CommandPool cmd_pool{};
	cmd_pool.setDevice(_device);
	cmd_pool.setQueueFamilyIndex(getFamilyIndex());
	cmd_pool.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	cmd_pool.create();

	_cmd_pools[_queue].push_back(cmd_pool);
	_cmd_pool_i = _cmd_pools[_queue].size() - 1;
}

CommandBuffer& Queue::allocateCommandBuffer(VkCommandBufferLevel level) {
	CommandBuffer cmd_buf;

	cmd_buf.setDevice(getDevice());
	cmd_buf.setCommandPool(&getCommandPool());
	cmd_buf.setLevel(level);
	cmd_buf.allocate();

	
	_pending_command_buffers[&getCommandPool()].push_back(cmd_buf.getCommandBuffer());
	_command_pool_indices[&getCommandPool()].nb_cmd_buf++;

	return cmd_buf;
}

//void Queue::copy(Buffer src, Buffer dst, VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT) {
//	CommandBuffer cmd_buf = _cmd_pool.allocateCommandBuffer();
//	cmd_buf.end();
//
//	VkBufferCopy2 region{};
//	region.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
//	region.pNext = nullptr;
//	region.srcOffset = 0;
//	region.dstOffset = 0;
//	region.size = src.getSize();
//
//	VkCopyBufferInfo2 info{};
//	info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
//	info.pNext = nullptr;
//	info.srcBuffer = src.getBuffer();
//	info.dstBuffer = dst.getBuffer();
//	info.regionCount = 1;
//	info.pRegions = &region;
//
//	vkCmdCopyBuffer2(
//		cmd_buf.getCommandBuffer(),
//		&info
//	);
//
//	cmd_buf.pipelineBarrier(
//		src_stage_mask,
//		dst_stage_mask,
//		0,
//		0,
//		nullptr,
//		0,
//		nullptr,
//		0,
//		nullptr
//	);
//
//	cmd_buf.end();
//
//	_pending_command_buffers.push_back(cmd_buf.getCommandBuffer());
//}
//
//void Queue::changeLayout(Image img, VkImageLayout) {
//
//}
//
//void Queue::changeToTexture(Image img) {
//
//}
//
//void Queue::changeToSurface(Image img) {
//
//}

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

	getStartIndexPendingCommendBuffers() += getNbPendingCommandBuffers();
	getNbPendingCommandBuffers() = 0;
}

const void Queue::submitSync(
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
		_fence
	);

	vkResetFences(
		_device->getDevice(),
		1,
		&_fence
	);

	getStartIndexPendingCommendBuffers() = 0;
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

VkCommandBuffer* Queue::getPendingCommandBufferStartPointer() const {
	return &_pending_command_buffers[&getCommandPool()][_command_pool_indices[&getCommandPool()].start_i];
}

uint32_t& Queue::getStartIndexPendingCommendBuffers() {
	return _command_pool_indices[&getCommandPool()].start_i;
}

uint32_t& Queue::getNbPendingCommandBuffers() {
	return _command_pool_indices[&getCommandPool()].nb_cmd_buf;
}