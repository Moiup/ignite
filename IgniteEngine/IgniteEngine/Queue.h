#pragma once

#include "PhysicalDevice.h"
#include "CommandPool.h"
#include <vector>

class Queue
{
private:
	VkQueue _queue;
	VkDevice _device;
	CommandPool _cmd_pool;

	uint32_t  _family_index{};

public:
	Queue();

	void setDevice(VkDevice device);
	void setFamilyIndex(uint32_t family_index);
	void setQueue();
	void createCommandPool();

	CommandPool& getCommandPool();

	VkQueue getQueue();
	VkDevice getDevice();
	
	uint32_t getFamilyIndex();

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

