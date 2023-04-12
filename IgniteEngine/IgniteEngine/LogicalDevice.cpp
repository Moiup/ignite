#include "LogicalDevice.h"


LogicalDevice::LogicalDevice() : 
	_logical_device{},
	_gpu{nullptr}
{
	;
}

void LogicalDevice::configure(const VkPhysicalDevice *gpu) {
	_gpu = gpu;

	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties2(
		*_gpu,
		&family_count,
		nullptr
	);
	_family_properties.resize(family_count);
	for (auto& prop : _family_properties) {
		prop.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
	}
	vkGetPhysicalDeviceQueueFamilyProperties2(
		*_gpu,
		&family_count,
		_family_properties.data()
	);
}

bool LogicalDevice::setQueue(std::string name, std::vector<VkQueueFlagBits> flags){
	uint32_t family_index{0};
	bool is_flag{false};

	if (_queues.count(name)) {
		return false;
	}

	for (uint32_t i = 0; i < _family_properties.size(); i++) {
		is_flag = isFlag(_family_properties[i].queueFamilyProperties.queueFlags, flags);
		if(is_flag) {
			family_index = i;
			break;
		}
	}

	if (!is_flag) {
		return false;
	}

	// Create the VkDeviceQueueCreateInfo
	float queue_priorities = 1.0f;
	Queue queue{};
	queue.setQueueFamilyIndex(family_index);

	_queues.insert({ name, queue });

	return true;
}

const Queue* LogicalDevice::getQueue(std::string name) const {
	return &_queues.at(name);
}

const Queue* LogicalDevice::getDefaultQueue() const {
	return &_queues.begin()->second;
}

const std::vector<uint32_t>& LogicalDevice::getQueueFamilyIndexes() const {
	return _queue_family_indexes;
}

void LogicalDevice::create() {
	std::vector<VkDeviceQueueCreateInfo> queues_info;
	createQueueCreateInfo(queues_info);
	createLogicalDevice(queues_info);
	gettingTheQueues();
}

void LogicalDevice::destroy() {
	vkDestroyDevice(_logical_device, nullptr);
}

const VkDevice* LogicalDevice::getDevice() const {
	return &_logical_device;
}

void LogicalDevice::waitIdle() {
	vkDeviceWaitIdle(_logical_device);
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

void LogicalDevice::createQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& queues_info) {
	std::set<uint32_t> queues_index;
	for (auto& q : _queues) {
		uint32_t index = q.second.getInfos()->queueFamilyIndex;
		if (queues_index.count(index)) {
			continue;
		}

		queues_index.insert(index);
		queues_info.push_back(*q.second.getInfos());
		_queue_family_indexes.push_back(index);
	}
}

void LogicalDevice::createLogicalDevice(std::vector<VkDeviceQueueCreateInfo> &queues_info) {
	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{};
	dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamic_rendering_features.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceFeatures device_features{};
	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = &dynamic_rendering_features;
	device_info.flags = 0;
	device_info.queueCreateInfoCount = queues_info.size();
	device_info.pQueueCreateInfos = queues_info.data();
	device_info.enabledExtensionCount = _EXTENSIONS.size();
	device_info.ppEnabledExtensionNames = _EXTENSIONS.data();
	device_info.pEnabledFeatures = &device_features;

	VkResult vk_result = vkCreateDevice(
		*_gpu,
		&device_info,
		nullptr,
		&_logical_device
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating logical device.");
	}
}

void LogicalDevice::gettingTheQueues() {
	// Getting the queues
	for (auto& q : _queues) {
		vkGetDeviceQueue(
			_logical_device,
			q.second.getInfos()->queueFamilyIndex,
			0,
			q.second.getQueue()
		);
	}
}