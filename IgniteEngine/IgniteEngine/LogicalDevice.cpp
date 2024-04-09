#include "LogicalDevice.h"


LogicalDevice::LogicalDevice()
{
	;
}

void LogicalDevice::configure(const PhysicalDevice *gpu) {
	_gpu = gpu;

	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties2(
		_gpu->getGPU(),
		&family_count,
		nullptr
	);
	_family_properties.resize(family_count);
	for (auto& prop : _family_properties) {
		prop.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
	}
	vkGetPhysicalDeviceQueueFamilyProperties2(
		_gpu->getGPU(),
		&family_count,
		_family_properties.data()
	);
}

bool LogicalDevice::setQueue(std::string name, std::vector<VkQueueFlagBits> flags, uint32_t count){
	uint32_t family_index{0};
	bool is_flag{false};

	if (_queues.count(name)) {
		return false;
	}

	for (uint32_t i = 0; i < _family_properties.size(); i++) {
		is_flag = isFlag(_family_properties[i].queueFamilyProperties.queueFlags, flags);
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

std::vector<Queue>& LogicalDevice::getQueues(const std::string& name) {
	return _queues[name];
}

void LogicalDevice::create() {
	std::vector<VkDeviceQueueCreateInfo> queues_info{};
	std::unordered_map<std::string, std::vector<float>> priorities{};
	createQueueCreateInfo(queues_info, priorities);
	createLogicalDevice(queues_info);
	gettingTheQueues();
}

void LogicalDevice::destroy() {
	vkDestroyDevice(_device, nullptr);
}

const VkDevice LogicalDevice::getDevice() const {
	return _device;
}

void LogicalDevice::waitIdle() {
	vkDeviceWaitIdle(_device);
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

void LogicalDevice::createLogicalDevice(std::vector<VkDeviceQueueCreateInfo> &queues_info) {
	VkPhysicalDeviceVulkan12Features physical_device_v12_features{};
	physical_device_v12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	physical_device_v12_features.runtimeDescriptorArray = VK_TRUE;

	VkPhysicalDeviceFeatures physical_device_features{};
	VkPhysicalDeviceFeatures2 physical_device_features2{};
	physical_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	physical_device_features2.pNext = &physical_device_v12_features;
	physical_device_features2.features = physical_device_features;

	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{};
	dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	dynamic_rendering_features.pNext = &physical_device_features2;
	dynamic_rendering_features.dynamicRendering = VK_TRUE;

	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = &dynamic_rendering_features;
	device_info.flags = 0;
	device_info.queueCreateInfoCount = queues_info.size();
	device_info.pQueueCreateInfos = queues_info.data();
	device_info.enabledExtensionCount = _EXTENSIONS.size();
	device_info.ppEnabledExtensionNames = _EXTENSIONS.data();
	device_info.pEnabledFeatures = nullptr;

	VkResult vk_result = vkCreateDevice(
		_gpu->getGPU(),
		&device_info,
		nullptr,
		&_device
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating logical device.");
	}
}

void LogicalDevice::gettingTheQueues() {
	// Getting the queues
	for (auto& q : _queues) {
		
	}
}