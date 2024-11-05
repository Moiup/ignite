#pragma once

#include "PhysicalDevice.h"

#include <vector>
#include <string>

class Device
{
private:
	const std::vector<const char*> _EXTENSIONS{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	PhysicalDevice* _gpu{ nullptr };
	VkDevice _device{};

	std::vector<VkQueueFamilyProperties2> _family_properties;

public:
	Device();

	void configure(PhysicalDevice* gpu);
	VkDevice getDevice();
	PhysicalDevice* getGPU();
	std::vector<VkQueueFamilyProperties2>& getFamilyProperties();

	void create(std::vector<VkDeviceQueueCreateInfo>& queues_info);
	void destroy();
};

