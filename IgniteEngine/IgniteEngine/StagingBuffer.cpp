#include "StagingBuffer.h"

StagingBuffer::StagingBuffer() {
	setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}
