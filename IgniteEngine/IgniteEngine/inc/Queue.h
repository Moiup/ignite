#pragma once

#include "PhysicalDevice.h"
#include "SharedCounter.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "Image.h"
#include "ComputePipeline.h"
#include "GraphicsPipeline.h"
#include <vector>
#include <list>
#include <unordered_map>

struct CommandPoolSubmitBuffersIndices {
	uint32_t start_i{0};
	uint32_t nb_cmd_buf{0};
};

class Queue
{
private:
	//static std::unordered_map<VkQueue, std::vector<CommandPool>> _cmd_pools;
	//static std::unordered_map<CommandPool*, std::vector<VkCommandBuffer>> _pending_command_buffers;
	//static std::unordered_map<CommandPool*, CommandPoolSubmitBuffersIndices> _command_pool_indices;

	VkQueue _queue{ nullptr };
	Device* _device{ nullptr };
	VkFence _fence{ nullptr };

	uint32_t _family_index{};
	uint32_t _queue_index{};

	int32_t *_shared_counter{nullptr};

public:
	Queue();
	Queue(
		Device& device,
		uint32_t family_index,
		uint32_t queue_index
	);
	Queue(const Queue& q);
	Queue(Queue&& q);

	~Queue();

	Queue& operator=(const Queue& q);
	Queue& operator=(Queue&& q);

	void setDevice(Device* device);
	void setFamilyIndex(uint32_t family_index);
	void create();

	VkQueue getQueue();
	Device* getDevice();
	uint32_t getFamilyIndex();

	CommandPool newCommandPool();

	void flush(
		const CommandPool& command_pool,
		const uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		const uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo = nullptr
	);

	void flush(
		std::vector<VkCommandBuffer> command_buffers,
		const uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		const uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo = nullptr
	);

	const void submit(
		const CommandPool& command_pool,
		const uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		const uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo = nullptr
	);

	const void submit(
		std::vector<VkCommandBuffer> command_buffers,
		const uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		const uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo = nullptr
	);

	const void submit(
		const uint32_t waitSemaphorecount,
		const VkSemaphore* pWaitSemaphores,
		const VkPipelineStageFlags* pWaitDstStageMask,
		const uint32_t commandBufferCount,
		const VkCommandBuffer* pCommandBuffers,
		const uint32_t signalSemaphoreCount,
		const VkSemaphore* pSignalSemaphores,
		VkFence fence,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
	);

	const void wait();

	const void present(
		uint32_t waitSemaphoreCount,
		const VkSemaphore* pWaitSemaphores,
		uint32_t swapchainCount,
		const VkSwapchainKHR* pSwapchains,
		const uint32_t* pImageIndices,
		VkResult* pResults = nullptr
	) const;
	
	void waitIdle();

private:
	void createFence(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);

	void destroy();
};
