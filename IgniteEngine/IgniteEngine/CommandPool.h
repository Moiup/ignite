#pragma once

#include "LogicalDevice.h"
#include "CommandBuffer.h"

class CommandPool
{
private:
	VkCommandPool _pool;
	VkDevice* _logical_device;
	VkCommandPoolCreateFlagBits _flags;
	uint32_t _family_index;
	bool _created;

public:
	CommandPool();

	void setLogicalDevice(VkDevice* logical_device);
	void setFlags(VkCommandPoolCreateFlagBits flags);
	void setQueueFamilyIndex(uint32_t family_index);
	void create();
	void destroy();

	const VkCommandPool& getPool() const;

	CommandBuffer createCommandBuffer(VkCommandBufferLevel level= VK_COMMAND_BUFFER_LEVEL_PRIMARY);
};

