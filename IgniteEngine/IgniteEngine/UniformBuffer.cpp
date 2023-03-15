#include "UniformBuffer.h"

UniformBuffer::UniformBuffer() :
	ArrayBuffer::ArrayBuffer()
{
	setPNext(nullptr);
	setFlags(0);
	setUsage(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setQueueFamilyIndexCount(0);
	setPQueueFamilyIndices(nullptr);
}

void UniformBuffer::create() {
	createBuffer();
	allocateMemory(
		*_memory_properties,
		0,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	bind();
}