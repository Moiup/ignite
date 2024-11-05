#pragma once

#include "Device.h"
#include "IGETypes.h"

#include <vulkan/vulkan.h>
#include <iostream>

struct PipelineStageAndAccessMaskInfo {
	VkPipelineStageFlags stage_mask;
	VkAccessFlags access_mask;
};

class Ressource
{
protected:
	Device* _device;
	VkMemoryRequirements _memory_req{};
	VkDeviceMemory _memory{};
	VkMemoryPropertyFlags _memory_property_flags{};

	PipelineStageAndAccessMaskInfo _stage_access_info;

protected:
	Ressource();
	Ressource(const Ressource& r);

	Ressource& operator=(const Ressource& r);

public:
	void setDevice(Device* device);
	void setMemoryPropertyFlags(VkMemoryPropertyFlags memory_property_flags);
	void allocateMemory();
	virtual void bind() = 0;

	Device* getDevice();
	const PipelineStageAndAccessMaskInfo& getStageAccessInfo() const;
	PipelineStageAndAccessMaskInfo& getStageAccessInfo();

	void freeMemory();

protected:
	virtual void getMemoryRequirements();
};

