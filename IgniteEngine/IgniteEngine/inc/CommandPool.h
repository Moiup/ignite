#pragma once

#include "Device.h"

class CommandPool
{
private:
	VkCommandPool _pool{ nullptr };
	Device* _device;
	VkCommandPoolCreateFlagBits _flags;
	uint32_t _family_index;

public:
	CommandPool();
	CommandPool(const CommandPool& cp);
	~CommandPool();

	void setDevice(Device* device);
	void setFlags(VkCommandPoolCreateFlagBits flags);
	void setQueueFamilyIndex(uint32_t family_index);
	void create();
	void reset();

	const VkCommandPool& getPool() const;

private:
	void destroy();
};

