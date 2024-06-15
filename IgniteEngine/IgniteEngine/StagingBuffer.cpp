#include "StagingBuffer.h"

StagingBuffer::StagingBuffer()
{
	Buffer::Buffer();
	setFlags(0);
	setPNext(nullptr);
	setQueueFamilyIndexCount(0);
	setPQueueFamilyIndices(nullptr);

	setUsage(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
