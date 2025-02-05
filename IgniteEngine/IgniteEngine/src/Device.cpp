#include "Device.h"

Device::Device() {
	_shared_count = new int32_t(1);
}

Device::Device(
	PhysicalDevice& gpu
) :
	Device::Device()
{
	create();
}

Device::Device(const Device& device) {
	*this = device;
}

Device::Device(Device&& device) {
	*this = std::move(device);
}

Device& Device::operator=(const Device& device) {
	destroy();
	_gpu = device._gpu;
	_device = device._device;
	_queue_family_infos = device._queue_family_infos;

	_shared_count = device._shared_count;
	*_shared_count += 1;
	
	return *this;
}

Device& Device::operator=(Device&& device) {
	destroy();
	_gpu = std::move(device)._gpu;
	device._gpu = nullptr;
	_device = std::move(device)._device;
	device._device = nullptr;
	_queue_family_infos = std::move(device._queue_family_infos);

	_shared_count = std::move(device)._shared_count;
	device._shared_count = nullptr;

	return *this;
}

Device::~Device() {
	destroy();
}

VkDevice Device::getDevice() {
	return _device;
}

PhysicalDevice* Device::getGPU() {
	return _gpu;
}

void Device::waitIdle() {
	vkDeviceWaitIdle(_device);
}


void Device::create() {
	VkPhysicalDeviceFeatures features = featuresManagement();

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT shader_atomic_float_feats{};
	shader_atomic_float_feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	shader_atomic_float_feats.pNext = nullptr;
	shader_atomic_float_feats.shaderBufferFloat32Atomics = VK_TRUE;
	shader_atomic_float_feats.shaderBufferFloat32AtomicAdd = VK_TRUE;
	shader_atomic_float_feats.shaderBufferFloat64Atomics = VK_TRUE;
	shader_atomic_float_feats.shaderBufferFloat64AtomicAdd = VK_TRUE;
	shader_atomic_float_feats.shaderSharedFloat32Atomics = VK_TRUE;
	shader_atomic_float_feats.shaderSharedFloat32AtomicAdd = VK_TRUE;
	shader_atomic_float_feats.shaderSharedFloat64Atomics = VK_TRUE;
	shader_atomic_float_feats.shaderSharedFloat64AtomicAdd = VK_TRUE;
	shader_atomic_float_feats.shaderImageFloat32Atomics = VK_TRUE;
	shader_atomic_float_feats.shaderImageFloat32AtomicAdd = VK_TRUE;
	shader_atomic_float_feats.sparseImageFloat32Atomics = VK_TRUE;
	shader_atomic_float_feats.sparseImageFloat32AtomicAdd = VK_TRUE;
	
	VkPhysicalDeviceVulkan13Features physical_device_v13_features{};
	physical_device_v13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	physical_device_v13_features.pNext = &shader_atomic_float_feats;
	physical_device_v13_features.synchronization2 = VK_TRUE;
	physical_device_v13_features.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceVulkan12Features physical_device_v12_features{};
	physical_device_v12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	physical_device_v12_features.pNext = &physical_device_v13_features;
	physical_device_v12_features.runtimeDescriptorArray = VK_TRUE;
	physical_device_v12_features.timelineSemaphore = VK_TRUE;

	VkPhysicalDeviceFeatures2 physical_device_features2{};
	physical_device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	physical_device_features2.pNext = &physical_device_v12_features;
	physical_device_features2.features = features;

	QueueCreationInfo queues_info = queueCreateInfos();

	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = &physical_device_features2;
	device_info.flags = 0;
	device_info.queueCreateInfoCount = queues_info.queue_create_infos.size();
	device_info.pQueueCreateInfos = queues_info.queue_create_infos.data();
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
		std::cerr << "Error: failed creating logical device." << std::endl;
		throw std::runtime_error("Error: failed creating logical device.");
	}
}

void Device::destroy() {
	if (!_shared_count) {
		return;
	}

	*_shared_count -= 1;
	if (*_shared_count) {
		return;
	}

	delete _shared_count;
	_shared_count = nullptr;

	if (!_device) {
		return;
	}
	vkDestroyDevice(_device, nullptr);
}

VkPhysicalDeviceFeatures Device::featuresManagement() {
	VkPhysicalDeviceFeatures2 available{};
	available.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	vkGetPhysicalDeviceFeatures2(getGPU()->getGPU(), &available);
	
	VkPhysicalDeviceFeatures enabled{};
	if (available.features.fillModeNonSolid) {
		enabled.fillModeNonSolid = VK_TRUE;
	}

	enabled.shaderFloat64 = VK_TRUE;
	enabled.fragmentStoresAndAtomics = VK_TRUE;
	enabled.wideLines = VK_TRUE;

	return enabled;
}

QueueCreationInfo Device::queueCreateInfos() {
	std::vector<VkQueueFamilyProperties2>  queue_family_properties = _gpu->getQueueFamilyProperties();

	uint32_t max_queue_count = 0;
	for (auto& qfp : queue_family_properties) {
		QueueFamilyInfo queue_family_info{};
		queue_family_info.properties = qfp;
		_queue_family_infos.push_back(queue_family_info);

		max_queue_count = std::max(
			max_queue_count,
			qfp.queueFamilyProperties.queueCount
		);
	}

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
	std::vector<float> priorities(max_queue_count, 0.0f);

	int32_t family_index = 0;
	for (auto& qfp : queue_family_properties) {
		VkDeviceQueueCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.pNext = nullptr;
		//info.flags = 0;
		info.queueFamilyIndex = family_index++;
		info.queueCount = qfp.queueFamilyProperties.queueCount;
		info.pQueuePriorities = priorities.data();
	}

	return {
		std::move(queue_create_infos),
		std::move(priorities)
	};
}