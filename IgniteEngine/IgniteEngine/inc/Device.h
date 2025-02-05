#pragma once

#include "PhysicalDevice.h"

#include <vector>
#include <string>

struct QueueCreationInfo {
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
	std::vector<float> priorities{};
};

struct QueueFamilyInfo {
	VkQueueFamilyProperties2 properties{};
	int32_t nb_queue_retreived{0};
};

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

	std::vector<QueueFamilyInfo> _queue_family_infos;

	int32_t* _shared_count{nullptr};

public:
	Device();
	Device(PhysicalDevice& gpu);

	Device(const Device& device);
	Device(Device&& device);

	Device& operator=(const Device& device);
	Device& operator=(Device&& device);

	~Device();

	VkDevice getDevice();
	PhysicalDevice* getGPU();

	void waitIdle();

private:
	void destroy();
	void create();
	VkPhysicalDeviceFeatures featuresManagement();
	QueueCreationInfo Device::queueCreateInfos();
};

