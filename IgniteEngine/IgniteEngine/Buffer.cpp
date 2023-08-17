#include "Buffer.h"

Buffer::Buffer() :
	Ressource::Ressource(),
	_buffer{nullptr},
	_buffer_info{}
{
	_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
}

void Buffer::create() {
	createBuffer();
}

void Buffer::destroy() {
	freeMemory();
	destroyBuffer();
}

void Buffer::setValues(void* values) {
	if (_buffer_info.size == 0) {
		return;
	}
	
	void* copy;
	VkResult vk_result = vkMapMemory(
		*_logical_device,
		_memory,
		0,
		_buffer_info.size,
		0,
		&copy
	);

	memcpy(copy, values, _buffer_info.size);
	vkUnmapMemory(*_logical_device, _memory);
}

void Buffer::bind() {
	VkResult vk_result = vkBindBufferMemory(
		*_logical_device,
		_buffer,
		_memory,
		0
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed binding the buffer!");
	}
}

void Buffer::setPNext(const void* p_next) {
	_buffer_info.pNext = p_next;
}

void Buffer::setFlags(VkBufferCreateFlags flags) {
	_buffer_info.flags = flags;
}

void Buffer::setSize(VkDeviceSize size) {
	_buffer_info.size = size;
}

void Buffer::setUsage(VkBufferUsageFlags usage) {
	_buffer_info.usage = usage;
}

void Buffer::setSharingMode(VkSharingMode sharing_mode) {
	_buffer_info.sharingMode = sharing_mode;
}

void Buffer::setQueueFamilyIndexCount(uint32_t family_index_count) {
	_buffer_info.queueFamilyIndexCount = family_index_count;
}

void Buffer::setPQueueFamilyIndices(const uint32_t* p_queue_family_indices){
	_buffer_info.pQueueFamilyIndices = p_queue_family_indices;
}

VkBuffer& Buffer::getBuffer() {
	return _buffer;
}

void* Buffer::getValues() {
	if (_buffer_info.size == 0) {
		return nullptr;
	}

	void* copy{};
	void* values = new uint8_t[_buffer_info.size];
	vkMapMemory(
		*_logical_device,
		_memory,
		0,
		_buffer_info.size,
		0,
		(void**)&copy
	);
	memcpy(values, copy, _buffer_info.size);
	vkUnmapMemory(*_logical_device, _memory);

	return values;	
}

void Buffer::createBuffer() {
	VkResult vk_result = vkCreateBuffer(
		*_logical_device,
		&_buffer_info,
		nullptr,
		&_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the buffer!");
	}
}

void Buffer::destroyBuffer() {
	vkDestroyBuffer(
		*_logical_device,
		_buffer,
		nullptr
	);
}

void Buffer::getMemoryRequirements() {
	vkGetBufferMemoryRequirements(
		*_logical_device,
		_buffer,
		&_memory_req
	);
}