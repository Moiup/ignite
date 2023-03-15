#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice() : _gpu{} {
	;
}

void PhysicalDevice::configure() {
	;
}

const VkPhysicalDevice& PhysicalDevice::getGPU() const {
	return _gpu;
}

void PhysicalDevice::setGPU(VkPhysicalDevice& gpu) {
	_gpu = { gpu };
	vkGetPhysicalDeviceMemoryProperties(_gpu, &_memory_properties);
}

VkPhysicalDeviceMemoryProperties* PhysicalDevice::getMemoryProperties() {
	return &_memory_properties;
}