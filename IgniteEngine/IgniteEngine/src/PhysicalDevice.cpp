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

VkFormatProperties2 PhysicalDevice::getFormatProperties(VkFormat format){
	VkFormatProperties2 f_prop;

	vkGetPhysicalDeviceFormatProperties2(
		_gpu,
		format,
		&f_prop
	);

	return f_prop;
}

std::vector<VkExtensionProperties> PhysicalDevice::enumerateDeviceExtensionProperties() {
	std::vector<VkExtensionProperties> extensions;
	uint32_t nb_ext;
	
	vkEnumerateDeviceExtensionProperties(
		_gpu,
		nullptr,
		&nb_ext,
		nullptr
	);
	extensions.resize(nb_ext);
	vkEnumerateDeviceExtensionProperties(
		_gpu,
		nullptr,
		&nb_ext,
		extensions.data()
	);

	return extensions;
}

void PhysicalDevice::displayDeviceExtensionProperties() {
	std::vector<VkExtensionProperties> extensions = enumerateDeviceExtensionProperties();

	std::cout << "Physical device extension properties (" << extensions.size() << ")" << std::endl;
	for (auto& ext : extensions) {
		std::cout << "    " << ext.extensionName << std::endl;
	}
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

void PhysicalDevice::displayQueueFamilyProperties() {
	std::vector<VkQueueFamilyProperties2> family_properties = std::move(getQueueFamilyProperties());

	std::cout << "Nb families: " << family_properties.size() << std::endl;
	for (int32_t i = 0; i < family_properties.size(); ++i) {
		VkQueueFamilyProperties p = family_properties[i].queueFamilyProperties;
		std::cout << "-------------------" << std::endl;
		std::cout << "family #" << i << std::endl;
		std::cout << "    Queue count: " <<  p.queueCount << std::endl;
		std::cout << "    Flags" << std::endl;
		std::cout << "        Graphics       : " << ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT)? "true" : "false") << std::endl;
		std::cout << "        Compute        : " << ((p.queueFlags & VK_QUEUE_COMPUTE_BIT) ? "true" : "false") << std::endl;
		std::cout << "        Transfer       : " << ((p.queueFlags & VK_QUEUE_TRANSFER_BIT) ? "true" : "false") << std::endl;
		std::cout << "        Sparse binding : " << ((p.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "true" : "false") << std::endl;
		std::cout << "        Video decode   : " << ((p.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) ? "true" : "false") << std::endl;
		std::cout << "        Video encode   : " << ((p.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) ? "true" : "false") << std::endl;
		std::cout << "        Optical flow nv: " << ((p.queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) ? "true" : "false") << std::endl;
	}
	
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