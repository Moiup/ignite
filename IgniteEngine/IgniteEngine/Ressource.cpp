#include "Ressource.h"

Ressource::Ressource()
{
	;
}

void Ressource::setQueue(Queue* queue) {
	_queue = queue;
}

void Ressource::setMemoryPropertyFlags(VkMemoryPropertyFlags memory_property_flags) {
	_memory_property_flags = memory_property_flags;
}

void Ressource::allocateMemory() {
	getMemoryRequirements();

	if (_memory_req.size == 0) {
		return;
	}

	VkMemoryAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocate_info.pNext = nullptr;
	allocate_info.allocationSize = _memory_req.size;
	allocate_info.memoryTypeIndex = 0;

	VkPhysicalDeviceMemoryProperties* mem_prop = _queue->getGPU()->getMemoryProperties();

	uint32_t memory_type_i = 0;
	for (memory_type_i = 0; memory_type_i < mem_prop->memoryTypeCount; memory_type_i++) {
		if ((_memory_req.memoryTypeBits & (1 << memory_type_i)) && (mem_prop->memoryTypes[memory_type_i].propertyFlags & _memory_property_flags) == _memory_property_flags) {
			allocate_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > mem_prop->memoryTypeCount) {
		throw std::runtime_error("Error: buffer memory type not found!");
	}

	VkResult vk_result = vkAllocateMemory(
		_queue->getDevice()->getDevice(),
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

Queue* Ressource::getQueue() {
	return _queue;
}

PipelineStageAndAccessMaskInfo Ressource::getStageAccessInfo() {
	return _stage_access_info;
}

void Ressource::freeMemory() {
	vkFreeMemory(_queue->getDevice()->getDevice(), _memory, nullptr);
}

void Ressource::getMemoryRequirements() {
	;
}