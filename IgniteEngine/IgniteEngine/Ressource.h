#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>
#include <iostream>

class Ressource
{
protected:
	Device* _device{ nullptr };
	VkMemoryRequirements _memory_req{};
	VkDeviceMemory _memory{};
	VkPhysicalDeviceMemoryProperties* _memory_properties{};
	VkMemoryPropertyFlags _memory_property_flags{};

public:
	Ressource();

public:
	void setMemoryProperties(VkPhysicalDeviceMemoryProperties* memory_properties);
	void setDevice(Device* logical_device);
	void setMemoryPropertyFlags(VkMemoryPropertyFlags memory_property_flags);
	void allocateMemory();
	virtual void bind();

	void freeMemory();

protected:
	virtual void getMemoryRequirements();
};

