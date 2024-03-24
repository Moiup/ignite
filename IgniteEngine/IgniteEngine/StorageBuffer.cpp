#include "StorageBuffer.h"

StorageBuffer::StorageBuffer():
	ArrayBuffer::ArrayBuffer()
{
	setPNext(nullptr);
	setFlags(0);
	setUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setQueueFamilyIndexCount(0);
	setPQueueFamilyIndices(nullptr);
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}