#pragma once
#include "Instance.h"

class PhysicalDevice
{
private:
	VkPhysicalDevice _gpu;
	VkPhysicalDeviceMemoryProperties _memory_properties;

public:
	PhysicalDevice();

	virtual void configure();

	const VkPhysicalDevice& getGPU() const;
	void setGPU(VkPhysicalDevice& gpu);
	VkPhysicalDeviceMemoryProperties* getMemoryProperties();
};

