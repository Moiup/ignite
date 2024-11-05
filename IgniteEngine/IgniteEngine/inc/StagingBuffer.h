#pragma once

#include "StagingBuffer.hpp"

template<IGEBufferUsage U>
StagingBuffer<U>::StagingBuffer(
	Device* device,
	VkDeviceSize size
) :
	Buffer<U>(device, size)
{
	setMemoryPropertyFlags(
		IGEMemoryType::staging
	);
	create();
}

template<IGEBufferUsage U>
StagingBuffer<U>::StagingBuffer(
	Device* device,
	VkDeviceSize size,
	void* values
) :
	StagingBuffer<U>(device, size)
{
	setValues(values);
}

template<IGEBufferUsage U>
void* StagingBuffer<U>::map() {
	if (_size == 0) {
		return nullptr;
	}

	void* ptr{ nullptr };
	vkMapMemory(
		_device->getDevice(),
		_memory,
		0,
		_size,
		0,
		&ptr
	);

	return ptr;
}

template<IGEBufferUsage U>
void StagingBuffer<U>::unmap() {
	vkUnmapMemory(
		_device->getDevice(),
		_memory
	);
}

template<IGEBufferUsage U>
void StagingBuffer<U>::setValues(void* values) {
	if (_buffer_info.size == 0) {
		return;
	}

	void* copy;
	copy = map();
	memcpy(copy, values, _size);
	unmap();
}

template<IGEBufferUsage U>
Pointer<uint8_t> StagingBuffer<U>::getValues() {
	if (_size == 0) {
		return nullptr;
	}

	void* copy{};
	uint8_t* v = new uint8_t[_size];
	copy = map<U>();
	memcpy(v, copy, _size);
	unmap();

	return Pointer<uint8_t>(v);
}

template<IGEBufferUsage U>
void StagingBuffer<U>::getValues(Pointer<uint8_t> data) {
	if (_size == 0) {
		return;
	}

	void* copy{};
	copy = map();
	memcpy(data.data(), copy, _size);
	unmap();
}