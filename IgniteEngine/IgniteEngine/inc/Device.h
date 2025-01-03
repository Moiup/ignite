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
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME
	};

	PhysicalDevice* _gpu{ nullptr };
	VkDevice _device{};

	std::vector<VkQueueFamilyProperties2> _family_properties;

	int32_t* _shared_count{nullptr};

public:
	Device();
	Device(
		PhysicalDevice& gpu,
		std::vector<VkDeviceQueueCreateInfo>& queues_info
	);

	Device(const Device& device);
	Device(Device&& device);

	Device& operator=(const Device& device);
	Device& operator=(Device&& device);

	~Device();

	VkDevice getDevice();
	PhysicalDevice* getGPU();
	std::vector<VkQueueFamilyProperties2>& getFamilyProperties();

	void waitIdle();

private:
	void destroy();
	void configure(PhysicalDevice* gpu);
	void create(std::vector<VkDeviceQueueCreateInfo>& queues_info);
	VkPhysicalDeviceFeatures featuresManagement();
};

