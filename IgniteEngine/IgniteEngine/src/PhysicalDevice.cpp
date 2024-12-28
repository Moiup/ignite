#include "PhysicalDevice.h"

PhysicalDevice::PhysicalDevice() {
	;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice gpu) :
	_gpu{gpu}
{
	;
}

VkPhysicalDeviceProperties PhysicalDevice::getProperties() {
	VkPhysicalDeviceProperties2 prop{};
	prop.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	prop.pNext = nullptr;
	prop.properties = {};
	vkGetPhysicalDeviceProperties2(
		_gpu,
		&prop
	);

	return prop.properties;
}

void PhysicalDevice::displayProperties() {
	VkPhysicalDeviceProperties prop = getProperties();

	std::cout << "    " << prop.deviceName << std::endl;
	std::cout << "    " << makeString(prop.deviceType) << std::endl;
}

const VkPhysicalDevice& PhysicalDevice::getGPU() const {
	return _gpu;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::getMemoryProperties() {
	VkPhysicalDeviceMemoryProperties2 mem_prop{};
	mem_prop.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
	mem_prop.pNext = nullptr;
	mem_prop.memoryProperties = {};

	vkGetPhysicalDeviceMemoryProperties2(
		_gpu,
		&mem_prop
	);

	return mem_prop.memoryProperties;
}

std::vector<VkQueueFamilyProperties2> PhysicalDevice::getQueueFamilyProperties() {
	std::vector<VkQueueFamilyProperties2> family_properties{};

	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties2(
		_gpu,
		&family_count,
		nullptr
	);
	family_properties.resize(family_count);
	for (auto& prop : family_properties) {
		prop.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
	}
	vkGetPhysicalDeviceQueueFamilyProperties2(
		_gpu,
		&family_count,
		family_properties.data()
	);

	return family_properties;
}

std::string PhysicalDevice::makeString(VkPhysicalDeviceType type) {
	switch (type) {
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		return "OTHER";
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		return "INTEGRATED GPU";
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		return "DISCRETE GPU";
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		return "VIRSTUAL GPU";
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return "CPU";
	default:
		return "UNKNOWN";
	}
}