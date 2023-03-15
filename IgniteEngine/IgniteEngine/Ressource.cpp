#include "Ressource.h"

Ressource::Ressource() :
	_logical_device{nullptr},
	_memory_req{},
	_memory{}
{
	;
}

void Ressource::setLogicalDevice(VkDevice* logical_device) {
	_logical_device = logical_device;
}

void Ressource::setMemoryProperties(VkPhysicalDeviceMemoryProperties* memory_properties) {
	_memory_properties = memory_properties;
}

void Ressource::allocateMemory(VkPhysicalDeviceMemoryProperties memory_properties, uint32_t memory_type_index, VkMemoryPropertyFlags memory_property_flags) {
	getMemoryRequirements();

	if (_memory_req.size == 0) {
		return;
	}

	VkMemoryAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.pNext = nullptr;
	allocate_info.allocationSize = _memory_req.size;
	allocate_info.memoryTypeIndex = memory_type_index;

	uint32_t memory_type_i = 0;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			allocate_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: buffer memory type not found!");
	}

	VkResult vk_result = vkAllocateMemory(
		*_logical_device,
		&allocate_info,
		nullptr,
		&_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating buffer memory!");
	}
}

void Ressource::bind() {
	;
}

void Ressource::freeMemory() {
	vkFreeMemory(*_logical_device, _memory, nullptr);
}

void Ressource::getMemoryRequirements() {
	;
}