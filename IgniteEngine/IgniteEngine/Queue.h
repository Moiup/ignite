#pragma once

#include "PhysicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
//#include "Buffer.h"
//#include "Image.h"
#include <vector>
#include <unordered_map>

struct CommandPoolSubmitBuffersIndices {
	uint32_t start_i;
	uint32_t nb_cmd_buf;
};

class Queue
{
private:
	static std::unordered_map<VkQueue, std::vector<CommandPool>> _cmd_pools;
	static std::unordered_map<CommandPool*, std::vector<VkCommandBuffer>> _pending_command_buffers;
	static std::unordered_map<CommandPool*, CommandPoolSubmitBuffersIndices> _command_pool_indices;

	VkQueue _queue{ nullptr };
	Device* _device{ nullptr };
	VkFence _fence{ nullptr };
	PhysicalDevice* _gpu{ nullptr };
	uint32_t _cmd_pool_i{ 0 };

	uint32_t _family_index{};

public:
	Queue();
	Queue(Queue& q);
	Queue(const Queue& q);
	//Queue(Queue&& q);

	Queue& operator=(const Queue& q);

	void setDevice(Device* device);
	void setFamilyIndex(uint32_t family_index);
	//void setCommandPoolIndex(uint32_t cmd_pool_i);
	void setGPU(PhysicalDevice* gpu);
	void create();

	std::vector<CommandPool>& getCommandPools();
	CommandPool& getCommandPool() const;
	VkQueue getQueue();
	Device* getDevice();
	uint32_t getFamilyIndex();
	uint32_t getCommandPoolIndex();
	PhysicalDevice* getGPU();
	std::vector<VkCommandBuffer>& getPendingCommandBuffers();
	
	

	void addCommandPool(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);

	CommandBuffer allocateCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	//void copy(Buffer& src, Buffer& dst,
	//	VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	//);

	//void copySync(Buffer& src, Buffer& dst,
	//	VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	//);

	//void changeLayout(Image img, VkImageLayout layout,
	//	VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	//);
	//void changeToTexture(Image img);
	//void changeToSurface(Image img);

	void flush();

	const void submit(
		uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr
	);

	const void submitNoFence(
		uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr
	);

	const void submit(
		uint32_t waitSemaphorecount,
		const VkSemaphore* pWaitSemaphores,
		const VkPipelineStageFlags* pWaitDstStageMask,
		uint32_t commandBufferCount,
		const VkCommandBuffer* pCommandBuffers,
		uint32_t signalSemaphoreCount,
		const VkSemaphore* pSignalSemaphores,
		VkFence fence
	) const;

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
	void createFence();

	VkCommandBuffer* getPendingCommandBufferStartPointer() const;
	uint32_t& getStartIndexPendingCommendBuffers();
	uint32_t& getNbPendingCommandBuffers();
	//void flush(VkFence* fences);
};

