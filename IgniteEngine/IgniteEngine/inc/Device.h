#pragma once

#include "PhysicalDevice.h"
//#include "Queue.h"

#include <vector>
#include <string>
#include <optional>


struct QueueCreationInfo {
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
	std::vector<float> priorities{};
};

struct QueueFamilyInfo {
	VkQueueFamilyProperties2 properties{};
	int32_t nb_queue_retreived{0};
};

class Queue;

class Device : public VulkanObject<VkDevice>
{
private:
	const std::vector<const char*> _EXTENSIONS{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
		VK_EXT_DEVICE_FAULT_EXTENSION_NAME,
		VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME
		//VK_KHR_MAINTENANCE_5_EXTENSION_NAME
	};

	PhysicalDevice* _gpu{ nullptr };
	VkDevice _device{};

	std::vector<QueueFamilyInfo> _queue_family_infos;

	int32_t* _shared_count{nullptr};

	static PFN_vkCmdSetCheckpointNV _vkCmdSetCheckpointNV;
	static PFN_vkGetQueueCheckpointData2NV _vkGetQueueCheckpointData2NV;

public:
	Device();
	Device(PhysicalDevice& gpu);

	Device(const Device& device);
	Device(Device&& device);

	Device& operator=(const Device& device);
	Device& operator=(Device&& device);

	~Device();

	const VkDevice& vkObj() const;
	PhysicalDevice* getGPU();
	std::optional<Queue> getQueue(VkQueueFlags flags);
	std::optional<Queue> getQueueFromFamily(int32_t family_index);

	void waitIdle();

	static void getQueueCheckpointData2NV(
		VkQueue queue,
		uint32_t* pCheckpointDataCount,
		VkCheckpointData2NV* pCheckpointData
	);

	static void cmdSetCheckpointNV(
    	VkCommandBuffer commandBuffer,
    	const void* pCheckpointMarker
	);

private:
	void destroy();
	void create();
	void loadVulkanExtensionFunctions();
	VkPhysicalDeviceFeatures featuresManagement();
	QueueCreationInfo queueCreateInfos();
};

