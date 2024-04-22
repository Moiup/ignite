#pragma once

#include "Device.h"

class CommandPool
{
private:
	VkCommandPool _pool;
	Device* _device;
	VkCommandPoolCreateFlagBits _flags;
	uint32_t _family_index;
	bool _created;

public:
	CommandPool();

	void setDevice(Device* device);
	void setFlags(VkCommandPoolCreateFlagBits flags);
	void setQueueFamilyIndex(uint32_t family_index);
	void create();
	void destroy();

	const VkCommandPool& getPool() const;
};

