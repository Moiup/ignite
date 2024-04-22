#pragma once
#include "Device.h"
#include "Queue.h"

#include <unordered_map>
#include <set>
#include <string>

class LogicalDevice
{
private:
	Device _device;
	PhysicalDevice* _gpu;
	// <Name of the queue, <The queue, the Queue info>>
	std::unordered_map<std::string, std::vector<Queue>> _queues;
	std::vector<uint32_t> _queue_family_indexes;

public:
	LogicalDevice();

	void configure(PhysicalDevice* gpu);
	bool defineQueue(std::string name, std::vector<VkQueueFlagBits> flags, uint32_t count);
	std::vector<Queue>& getQueues(const std::string& name);

	void create();

	Device* getDevice();

	void waitIdle();

private:
	bool isFlag(VkQueueFlags family_flags, std::vector<VkQueueFlagBits> flags);

	void createQueueCreateInfo(
		std::vector<VkDeviceQueueCreateInfo>& queues_info,
		std::unordered_map<uint32_t, std::vector<float>>& priorities
	);
	void createLogicalDevice(std::vector<VkDeviceQueueCreateInfo> &queues_info);
	void gettingTheQueues();
};

