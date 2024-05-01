#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>
#include <iostream>

class DefaultConf;

class Ressource
{
protected:
	Queue* _queue;
	VkMemoryRequirements _memory_req{};
	VkDeviceMemory _memory{};
	VkMemoryPropertyFlags _memory_property_flags{};

public:
	Ressource();

public:
	void setQueue(Queue* queue);
	void setMemoryPropertyFlags(VkMemoryPropertyFlags memory_property_flags);
	void allocateMemory();
	virtual void bind();

	// Copy the ressource data in parameter into the calling ressource
	//virtual void copy() = 0;

	void freeMemory();

protected:
	virtual void getMemoryRequirements();
};

