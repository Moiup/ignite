#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include <iostream>
#include <string>

class Instance;

class PhysicalDevice
{
	friend Instance;

private:
	VkPhysicalDevice _gpu{nullptr};

public:
	PhysicalDevice();

	VkPhysicalDeviceProperties getProperties();
	void displayProperties();

	const VkPhysicalDevice& getGPU() const;
	VkPhysicalDeviceMemoryProperties getMemoryProperties();
	std::vector<VkQueueFamilyProperties2> getQueueFamilyProperties();
	void displayQueueFamilyProperties();

private:
	PhysicalDevice(VkPhysicalDevice gpu);
	std::string makeString(VkPhysicalDeviceType type);
};

