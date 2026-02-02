#include "Queue.h"

Queue::Queue()
{
	_shared_counter = new int32_t(1);
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

Queue::Queue(const Queue& q){
	*this = q;
}

Queue::Queue(Queue&& q) {
	*this = std::move(q);
}

Queue::~Queue() {
	destroy();
}

Queue& Queue::operator=(Queue&& q){
	destroy();

	_queue = std::move(q)._queue;
	_device = std::move(q)._device;
	_fence = std::move(q)._fence;
	_family_index = std::move(q)._family_index;
	_queue_index = std::move(q)._queue_index;
	_shared_counter = std::move(q)._shared_counter;
	q._shared_counter = nullptr;

	return *this;
}

Queue& Queue::operator=(const Queue& q){
	destroy();

	_queue = q._queue;
	_device = q._device;
	_fence = q._fence;
	_family_index = q._family_index;
	_queue_index = q._queue_index;
	_shared_counter = q._shared_counter;
	*_shared_counter += 1;

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
		_device->vkObj(),
		_family_index,
		_queue_index,
		&_queue
	);

	createFence();
}

const VkQueue& Queue::vkObj() const {
	return _queue;
}

Device* Queue::getDevice() {
	return _device;
}

uint32_t Queue::getFamilyIndex() {
	return _family_index;
}

CommandPool Queue::newCommandPool(){
	return CommandPool(
		*_device,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		_family_index
	);
}

void Queue::destroy() {
	if(!_shared_counter){
		return;
	}

	*_shared_counter -= 1;
	if(*_shared_counter){
		return;
	}
	delete _shared_counter;

	if (_fence) {
		vkDestroyFence(
			_device->vkObj(),
			_fence,
			nullptr
		);
		_fence = nullptr;
	}
}

void Queue::flush(
	const CommandPool& command_pool,
	const uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	const uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		command_pool.vkCommandBuffers().size(),
		command_pool.vkCommandBuffers().data(),
		signalSemaphoreCount,
		pSignalSemaphores,
		nullptr,
		timelineSemaphoreSubmitInfo
	);
}

void Queue::flush(
	std::vector<VkCommandBuffer> command_buffers,
	const uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	const uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		command_buffers.size(),
		command_buffers.data(),
		signalSemaphoreCount,
		pSignalSemaphores,
		nullptr,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	const CommandPool& command_pool,
	const uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		command_pool,
		_fence,
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		signalSemaphoreCount,
		pSignalSemaphores,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	std::vector<VkCommandBuffer> command_buffers,
	const uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	const uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		command_buffers,
		_fence,
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		signalSemaphoreCount,
		pSignalSemaphores,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	const CommandPool& command_pool,
	const VkFence fence,
	const uint32_t waitSemaphorecount,
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
		command_pool.vkCommandBuffers().size(),
		command_pool.vkCommandBuffers().data(),
		signalSemaphoreCount,
		pSignalSemaphores,
		fence,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	std::vector<VkCommandBuffer> command_buffers,
	const VkFence fence,
	const uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	const uint32_t signalSemaphoreCount,
	const VkSemaphore* pSignalSemaphores,
	const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
) {
	submit(
		waitSemaphorecount,
		pWaitSemaphores,
		pWaitDstStageMask,
		command_buffers.size(),
		command_buffers.data(),
		signalSemaphoreCount,
		pSignalSemaphores,
		fence,
		timelineSemaphoreSubmitInfo
	);
}

const void Queue::submit(
	const uint32_t waitSemaphorecount,
	const VkSemaphore* pWaitSemaphores,
	const VkPipelineStageFlags* pWaitDstStageMask,
	const uint32_t commandBufferCount,
	const VkCommandBuffer* pCommandBuffers,
	const uint32_t signalSemaphoreCount,
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
}


const void Queue::wait() {
	VkResult vk_result = vkWaitForFences(
		_device->vkObj(),
		1, &_fence,
		VK_TRUE,
		UINT64_MAX
	);

	
	if (vk_result != VK_SUCCESS) {
		std::string res = string_VkResult(vk_result);
		throw std::runtime_error("Queue::submitSync: Error while waiting for the fence to finish. vkWaitForFences error: " + res + " (" + std::to_string(vk_result) + ").");
	}

	vkResetFences(
		_device->vkObj(),
		1,
		&_fence
	);
}

const void Queue::wait(VkFence fence) {
	VkResult vk_result = vkWaitForFences(
		_device->vkObj(),
		1, &fence,
		VK_TRUE,
		UINT64_MAX
	);

	
	if (vk_result != VK_SUCCESS) {
		VkDeviceFaultCountsEXT info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_FAULT_COUNTS_EXT,
			.pNext = nullptr,
		};
		
		Instance::getDeviceFaultInfoEXT(
			_device->vkObj(),
			&info,
			nullptr
		);

		std::cout << info.vendorBinarySize << std::endl;
		std::cout << info.addressInfoCount << std::endl;
		std::cout << info.vendorInfoCount << std::endl;

		std::string res = string_VkResult(vk_result);
		throw std::runtime_error("Queue::submitSync: Error while waiting for the fence to finish. vkWaitForFences error: " + res + " (" + std::to_string(vk_result) + ").");
	}

	vkResetFences(
		_device->vkObj(),
		1,
		&fence
	);
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
	present_info.swapchainCount = swapchainCount;
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
		_device->vkObj(),
		&fence_info,
		nullptr,
		&_fence
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating fence!");
	}

	vkResetFences(
		_device->vkObj(),
		1,
		&_fence
	);
}
