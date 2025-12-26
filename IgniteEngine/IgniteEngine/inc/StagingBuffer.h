#pragma once

#include "StagingBuffer.hpp"

template<IGEBufferUsage U>
StagingBuffer<U>::StagingBuffer(
	Device* device,
	VkDeviceSize size
) :
	Buffer<U>(device, size)
{
	this->setMemoryPropertyFlags(
		IGEMemoryType::staging
	);
	this->create();
}

template<IGEBufferUsage U>
template<class T>
StagingBuffer<U>::StagingBuffer(
	Device* device,
	VkDeviceSize size,
	const std::vector<T>& v
) :
	StagingBuffer<U>(device, size)
{
	setValues(v.data(), v.size() * sizeof(T));
}

template<IGEBufferUsage U>
StagingBuffer<U>::StagingBuffer(
	Device* device,
	VkDeviceSize size,
	const void* values,
	const int32_t values_size
) :
	StagingBuffer<U>(device, size)
{
	setValues(values, values_size);
}

template<IGEBufferUsage U>
void* StagingBuffer<U>::map() {
	if (this->_size == 0) {
		return nullptr;
	}

	if(_map_ptr){
		return _map_ptr;
	}

	vkMapMemory(
		this->_device->getDevice(),
		this->_memory,
		0,
		this->_size,
		0,
		&_map_ptr
	);

	return _map_ptr;
}

template<IGEBufferUsage U>
void StagingBuffer<U>::unmap() {
	if(!_map_ptr){
		return;
	}
	vkUnmapMemory(
		this->_device->getDevice(),
		this->_memory
	);
	_map_ptr = nullptr;
}

template<IGEBufferUsage U>
template<class T>
void StagingBuffer<U>::setValues(const std::vector<T>& v) {
	setValues(v.data(), v.size() * sizeof(T));
}

template<IGEBufferUsage U>
void StagingBuffer<U>::setValues(const void* values, const int32_t size) {
	if (this->_buffer_info.size == 0) {
		return;
	}

	void* copy;
	copy = map();
	memcpy(copy, values, std::min(static_cast<int32_t>(this->_size), size));
	unmap();
}

template<IGEBufferUsage U>
Pointer<uint8_t> StagingBuffer<U>::getValues() {
	if (this->_size == 0) {
		return nullptr;
	}

	void* copy{};
	uint8_t* v = new uint8_t[this->_size];
	copy = map();
	memcpy(v, copy, this->_size);
	unmap();

	return Pointer<uint8_t>(v);
}

template<IGEBufferUsage U>
void StagingBuffer<U>::getValues(Pointer<uint8_t> data) {
	if (this->_size == 0) {
		return;
	}

	void* copy{};
	copy = map();
	memcpy(data.data(), copy, this->_size);
	unmap();
}