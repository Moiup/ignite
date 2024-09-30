#pragma once

#include "LogicalDevice.h"

#include <vulkan/vulkan.h>
#include <iostream>

class DefaultConf;

struct PipelineStageAndAccessMaskInfo {
	VkPipelineStageFlags stage_mask;
	VkAccessFlags access_mask;
};

class Ressource
{
protected:
	Queue* _queue{ nullptr };
	VkMemoryRequirements _memory_req{};
	VkDeviceMemory _memory{};
	VkMemoryPropertyFlags _memory_property_flags{};

	PipelineStageAndAccessMaskInfo _stage_access_info;

public:
	Ressource();

public:
	void setQueue(Queue* queue);
	void setMemoryPropertyFlags(VkMemoryPropertyFlags memory_property_flags);
	void allocateMemory();
	virtual void bind() = 0;

	Queue* getQueue();
	PipelineStageAndAccessMaskInfo getStageAccessInfo();


	// Copy the ressource data in parameter into the calling ressource
	//virtual void copy() = 0;

	void freeMemory();

protected:
	virtual void getMemoryRequirements();
};

