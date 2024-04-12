#include "LogicalDevice.h"


LogicalDevice::LogicalDevice()
{
	;
}



bool LogicalDevice::setQueue(std::string name, std::vector<VkQueueFlagBits> flags, uint32_t count){
	uint32_t family_index{0};
	bool is_flag{false};

	if (_queues.count(name)) {
		return false;
	}

	auto& family_properties = _device->getFamilyProperties();
	for (uint32_t i = 0; i < family_properties.size(); i++) {
		is_flag = isFlag(family_properties[i].queueFamilyProperties.queueFlags, flags);
		if (is_flag) {
			family_index = i;
			break;
		}
	}

	if (!is_flag) {
		return false;
	}


	for (uint32_t q = 0; q < count; q++) {
		float queue_priorities = 1.0f;
		Queue queue{};
		queue.setDevice(_device);
		queue.setFamilyIndex(family_index);

		_queues[name].push_back(queue);
	}

	return true;
}

void LogicalDevice::setGPU(PhysicalDevice* gpu) {
	_gpu = gpu;
}

std::vector<Queue>& LogicalDevice::getQueues(const std::string& name) {
	return _queues[name];
}

void LogicalDevice::create() {
	std::vector<VkDeviceQueueCreateInfo> queues_info{};
	std::unordered_map<std::string, std::vector<float>> priorities{};
	createQueueCreateInfo(queues_info, priorities);
	_device->configure(_gpu);
	_device->create(queues_info);
	gettingTheQueues();
}

const Device* LogicalDevice::getDevice() const {
	return _device;
}

void LogicalDevice::waitIdle() {
	vkDeviceWaitIdle(_device->getDevice());
}

bool LogicalDevice::isFlag(VkQueueFlags family_flags, std::vector<VkQueueFlagBits> flags) {
	uint32_t i = 0;
	uint32_t n = flags.size();
	bool is_ok = true;

	do {
		is_ok = is_ok && (family_flags & flags[i]);
		i++;
	} while (is_ok && i < n);
	
	return is_ok;
}

void LogicalDevice::createQueueCreateInfo(
	std::vector<VkDeviceQueueCreateInfo>& queues_info,
	std::unordered_map<std::string, std::vector<float>> priorities
	) {

	for (auto& q : _queues) {
		std::vector<Queue>& queues = q.second;
		const std::string& name = q.first;
		if (queues.size()) {
			continue;
		}

		priorities[name] = std::vector<float>(queues.size(), 0.0f);

		VkDeviceQueueCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.queueFamilyIndex = q.second[0].getFamilyIndex();
		info.queueCount = queues.size();
		info.pQueuePriorities = priorities[name].data();

		queues_info.push_back(info);
	}
}

void LogicalDevice::gettingTheQueues() {
	// Getting the queues
	for (auto& q : _queues) {
		
	}
}