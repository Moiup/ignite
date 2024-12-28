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
	VkDeviceMemory _memory{nullptr};
	VkMemoryPropertyFlags _memory_property_flags{};

	PipelineStageAndAccessMaskInfo _stage_access_info;

	int32_t* _shared_count{nullptr};

public:
	Ressource();
	Ressource(const Ressource& r);
	Ressource(Ressource&& r);
	~Ressource();

	Ressource& operator=(const Ressource& r);
	Ressource& operator=(Ressource&& r);

	void setDevice(Device* device);
	void setMemoryPropertyFlags(VkMemoryPropertyFlags memory_property_flags);
	void allocateMemory();
	virtual void bind() = 0;

	Device* getDevice();
	const PipelineStageAndAccessMaskInfo& getStageAccessInfo() const;
	PipelineStageAndAccessMaskInfo& getStageAccessInfo();

	int32_t getNbShared();

private:
	void freeMemory();
	void destroy();

protected:
	virtual void getMemoryRequirements();
};

