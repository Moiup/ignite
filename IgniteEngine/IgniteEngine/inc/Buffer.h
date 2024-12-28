#pragma once

#include "Buffer.hpp"

template<IGEBufferUsage U>
Buffer<U>::Buffer() :
	Ressource::Ressource()
{

}

template<IGEBufferUsage U>
Buffer<U>::Buffer(
	Device* device,
	VkDeviceSize size
) :
	Buffer<U>()
{
	_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	setPNext(nullptr);
	setFlags(0);
	setUsage(U);
	setMemoryPropertyFlags(
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setQueueFamilyIndexCount(0);
	setPQueueFamilyIndices(nullptr);
	setDevice(device);
	setSize(size);
}

template<IGEBufferUsage U>
Buffer<U>::Buffer(const Buffer<U>& buf) {
	*this = buf;
}

template<IGEBufferUsage U>
Buffer<U>::Buffer(Buffer<U>&& buf) {
	*this = std::move(buf);
}

template<IGEBufferUsage U>
Buffer<U>::~Buffer() {
	destroy();
}

template<IGEBufferUsage U>
Buffer<U>& Buffer<U>::operator=(const Buffer<U>& buf) {
	destroy();
	Ressource::operator=(buf);
	_buffer = buf._buffer;
	_buffer_info = buf._buffer_info;
	_capacity = buf._capacity;
	_size = buf._size;

	return *this;
}

template<IGEBufferUsage U>
Buffer<U>& Buffer<U>::operator=(Buffer<U>&& buf) {
	destroy();
	Ressource::operator=(std::move(buf));
	_buffer = std::move(buf)._buffer;
	buf._buffer = nullptr;
	_buffer_info = std::move(buf)._buffer_info;
	_capacity = std::move(buf)._capacity;
	_size = std::move(buf)._size;

	return *this;
}

template<IGEBufferUsage U>
void Buffer<U>::create() {
	if (!_buffer_info.size) {
		return;
	}
	createBuffer();
	allocateMemory();
	bind();
}

template<IGEBufferUsage U>
void Buffer<U>::destroy() {
	if (getNbShared() > 1) {
		return;
	}

	if (!_buffer) {
		return;
	}
	destroyBuffer();
}

template<IGEBufferUsage U>
void Buffer<U>::bind() {
	VkResult vk_result = vkBindBufferMemory(
		_device->getDevice(),
		_buffer,
		_memory,
		0
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed binding the buffer!");
	}
}

template<IGEBufferUsage U>
void Buffer<U>::setPNext(const void* p_next) {
	_buffer_info.pNext = p_next;
}

template<IGEBufferUsage U>
void Buffer<U>::setFlags(VkBufferCreateFlags flags) {
	_buffer_info.flags = flags;
}

template<IGEBufferUsage U>
void Buffer<U>::setSize(VkDeviceSize size) {
	if (size > _capacity) {
		_capacity = size;
		_buffer_info.size = _capacity;
	}
	_size = size;
}

template<IGEBufferUsage U>
void Buffer<U>::setCapacity(VkDeviceSize capacity) {
	if (capacity < _buffer_info.size) {
		std::cerr << "Buffer<U>::setCapacity: Error! Capacity must be superior to size." << std::endl;
		return;
	}

	_capacity = capacity;
	_buffer_info.size = _capacity;
}

template<IGEBufferUsage U>
void Buffer<U>::setUsage(VkBufferUsageFlags usage) {
	_buffer_info.usage = usage;
}

template<IGEBufferUsage U>
void Buffer<U>::setSharingMode(VkSharingMode sharing_mode) {
	_buffer_info.sharingMode = sharing_mode;
}

template<IGEBufferUsage U>
void Buffer<U>::setQueueFamilyIndexCount(uint32_t family_index_count) {
	_buffer_info.queueFamilyIndexCount = family_index_count;
}

template<IGEBufferUsage U>
void Buffer<U>::setPQueueFamilyIndices(const uint32_t* p_queue_family_indices) {
	_buffer_info.pQueueFamilyIndices = p_queue_family_indices;
}

template<IGEBufferUsage U>
const VkBuffer& Buffer<U>::getBuffer() const {
	return _buffer;
}

template<IGEBufferUsage U>
const uint32_t& Buffer<U>::getSize() const {
	return _size;
}

template<IGEBufferUsage U>
const uint32_t& Buffer<U>::getCapacity() const {
	return _capacity;
}

template<IGEBufferUsage U>
void Buffer<U>::createBuffer() {
	VkResult vk_result = vkCreateBuffer(
		_device->getDevice(),
		&_buffer_info,
		nullptr,
		&_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the buffer!");
	}
}

template<IGEBufferUsage U>
void Buffer<U>::destroyBuffer() {
	vkDestroyBuffer(
		_device->getDevice(),
		_buffer,
		nullptr
	);
	_buffer = nullptr;
}

template<IGEBufferUsage U>
void Buffer<U>::getMemoryRequirements() {
	vkGetBufferMemoryRequirements(
		_device->getDevice(),
		_buffer,
		&_memory_req
	);
}