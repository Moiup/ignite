#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>
#include <iostream>

class Ressource
{
protected:
	LogicalDevice* _logical_device;
	VkMemoryRequirements _memory_req;
	VkDeviceMemory _memory;
	VkPhysicalDeviceMemoryProperties* _memory_properties;

public:
	Ressource();

public:
	void setMemoryProperties(VkPhysicalDeviceMemoryProperties* memory_properties);
	void setLogicalDevice(LogicalDevice* logical_device);
	void allocateMemory(VkMemoryPropertyFlags memory_property_flags);
	virtual void bind();

	void freeMemory();

protected:
	virtual void getMemoryRequirements();
};

