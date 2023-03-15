#pragma once

#include "PhysicalDevice.h"
#include <vector>

class Queue
{
private:
	VkQueue _queue;
	VkDeviceQueueCreateInfo _infos;

public:
	Queue();

	VkQueue* getQueue();

	void setQueueFamilyIndex(uint32_t index);
	
	const VkDeviceQueueCreateInfo* getInfos() const;
	const void submit(
		uint32_t waitSemaphorecount,
		const VkSemaphore* pWaitSemaphores,
		const VkPipelineStageFlags* pWaitDstStageMask,
		uint32_t commandBufferCount,
		const VkCommandBuffer* pCommandBuffers,
		uint32_t signalSemaphoreCount,
		const VkSemaphore* pSignalSemaphores,
		VkFence* fence
	) const;

	const void present(
		uint32_t waitSemaphoreCount,
		const VkSemaphore* pWaitSemaphores,
		uint32_t swapchainCount,
		const VkSwapchainKHR* pSwapchains,
		const uint32_t* pImageIndices,
		VkResult* pResults
	) const;
	
	
	void waitIdle();
};

