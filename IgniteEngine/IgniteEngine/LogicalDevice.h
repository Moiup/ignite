#pragma once
#include "PhysicalDevice.h"
#include "Queue.h"

#include <unordered_map>
#include <set>
#include <string>


class LogicalDevice
{
private:
	const std::vector<const char*> _EXTENSIONS{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	// <Name of the queue, <The queue, the Queue info>>
	std::unordered_map<std::string, Queue> _queues;
	std::vector<VkQueueFamilyProperties2> _family_properties;
	std::vector<uint32_t> _queue_family_indexes;

	const VkPhysicalDevice* _gpu;
	VkDevice _logical_device;

public:
	LogicalDevice();

	void configure(const VkPhysicalDevice* gpu);

	bool setQueue(std::string name, std::vector<VkQueueFlagBits> flags);
	const Queue* getQueue(std::string name) const;
	const Queue* getQueue(uint32_t i) const;
	const std::vector<uint32_t>& getQueueFamilyIndexes() const;

	void create();
	void destroy();

	const VkDevice* getDevice() const;

	void waitIdle();

private:
	bool isFlag(VkQueueFlags family_flags, std::vector<VkQueueFlagBits> flags);

	void createQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queues_info);
	void createLogicalDevice(std::vector<VkDeviceQueueCreateInfo> &queues_info);
	void gettingTheQueues();
};

