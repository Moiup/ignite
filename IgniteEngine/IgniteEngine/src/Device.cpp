#include "Device.h"

Device::Device() {
	;
}

VkDevice Device::getDevice() {
	return _device;
}

PhysicalDevice* Device::getGPU() {
	return _gpu;
}


void Device::configure(PhysicalDevice* gpu) {
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

std::vector<VkQueueFamilyProperties2>& Device::getFamilyProperties() {
	return _family_properties;
}

void Device::create(std::vector<VkDeviceQueueCreateInfo>& queues_info) {
	VkPhysicalDeviceFeatures features = featuresManagement();

	VkPhysicalDeviceShaderAtomicFloat2FeaturesEXT shader_atomic_float_2_feats;
	shader_atomic_float_2_feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_2_FEATURES_EXT;
	shader_atomic_float_2_feats.pNext = nullptr;
	shader_atomic_float_2_feats.shaderBufferFloat16Atomics = VK_TRUE;
	shader_atomic_float_2_feats.shaderBufferFloat16AtomicAdd = VK_TRUE;
	shader_atomic_float_2_feats.shaderBufferFloat16AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.shaderBufferFloat32AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.shaderBufferFloat64AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.shaderSharedFloat16Atomics = VK_TRUE;
	shader_atomic_float_2_feats.shaderSharedFloat16AtomicAdd = VK_TRUE;
	shader_atomic_float_2_feats.shaderSharedFloat16AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.shaderSharedFloat32AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.shaderSharedFloat64AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.shaderImageFloat32AtomicMinMax = VK_TRUE;
	shader_atomic_float_2_feats.sparseImageFloat32AtomicMinMax = VK_TRUE;

	VkPhysicalDeviceShaderAtomicFloatFeaturesEXT shader_atomic_float_feats{};
	shader_atomic_float_feats.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_FLOAT_FEATURES_EXT;
	shader_atomic_float_feats.pNext = &shader_atomic_float_2_feats;
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

	//VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_features{};
	//dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
	//dynamic_rendering_features.pNext = &physical_device_features2;
	//dynamic_rendering_features.dynamicRendering = VK_TRUE;

	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = &physical_device_features2;
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
		std::cerr << "Error: failed creating logical device." << std::endl;
		throw std::runtime_error("Error: failed creating logical device.");
	}
}

void Device::destroy() {
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

	return enabled;
}