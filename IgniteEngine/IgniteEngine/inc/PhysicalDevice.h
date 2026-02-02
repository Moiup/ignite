#pragma once
#include "vulkan/vulkan.h"
#include "VulkanObject.h"
#include <vector>
#include <iostream>
#include <string>

class Instance;

class PhysicalDevice : public VulkanObject<VkPhysicalDevice>
{
	friend Instance;

private:
	Instance* _instance;
	VkPhysicalDevice _gpu{nullptr};

public:
	PhysicalDevice();

	VkPhysicalDeviceProperties getProperties();
	void displayProperties();

	VkFormatProperties2 getFormatProperties(VkFormat format);

	std::vector<VkExtensionProperties> enumerateDeviceExtensionProperties();
	void displayDeviceExtensionProperties();

	const VkPhysicalDevice& vkObj() const;
	VkPhysicalDeviceMemoryProperties getMemoryProperties();
	std::vector<VkQueueFamilyProperties2> getQueueFamilyProperties();
	void displayQueueFamilyProperties();

private:
	PhysicalDevice(VkPhysicalDevice gpu, Instance& instance);
	std::string makeString(VkPhysicalDeviceType type);
};

