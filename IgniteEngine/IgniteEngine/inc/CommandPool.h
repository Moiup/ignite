#pragma once

#include <optional>
#include <memory>

#include "CommandBuffer.h"

#include "Device.h"

class CommandPool
{
private:
	VkCommandPool _pool{nullptr};
	Device* _device{ nullptr };
	VkCommandPoolCreateFlagBits _flags;
	uint32_t _family_index;

	std::vector<VkCommandBuffer> _vk_cmd_buffs;
	std::vector<CommandBuffer> _cmd_buffs;

	int32_t* _shared_count{nullptr};

public:
	CommandPool();
	CommandPool(
		Device& device,
		VkCommandPoolCreateFlagBits flags,
		uint32_t family_index
	);

	CommandPool(const CommandPool& cp);
	CommandPool(CommandPool&& cp);

	~CommandPool();

	CommandPool& operator=(const CommandPool& cp);
	CommandPool& operator=(CommandPool&& cp);

	bool operator==(const CommandPool& cp);

	void setDevice(Device* device);
	void setFlags(VkCommandPoolCreateFlagBits flags);
	void setQueueFamilyIndex(uint32_t family_index);
	void create();
	void reset();

	const VkCommandPool& getPool() const;

	CommandBuffer& newCommandBuffer();
	const std::vector<CommandBuffer>& commandBuffers() const;
	const std::vector<VkCommandBuffer>& vkCommandBuffers() const;

private:
	void destroy();
	void resetCommandBuffers();
};

