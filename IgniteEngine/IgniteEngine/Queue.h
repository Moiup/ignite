#pragma once

#include "PhysicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
//#include "Buffer.h"
//#include "Image.h"
#include <vector>

class Queue
{
private:
	VkQueue _queue;
	Device* _device;
	VkFence _fence;
	CommandPool _cmd_pool;

	uint32_t  _family_index{};

	std::vector<VkCommandBuffer> _pending_command_buffers;

public:
	Queue();

	void setDevice(Device* device);
	void setFamilyIndex(uint32_t family_index);
	void setQueue();

	CommandPool& getCommandPool();
	VkQueue getQueue();
	Device* getDevice();
	uint32_t getFamilyIndex();

	//void copy(Buffer src, Buffer dst, VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	//void changeLayout(Image img, VkImageLayout layout);
	//void changeToTexture(Image img);
	//void changeToSurface(Image img);

	void flush();

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

private:
	void createFence();
};

