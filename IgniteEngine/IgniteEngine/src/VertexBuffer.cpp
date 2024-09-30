#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() :
	Buffer::Buffer(),
	_first_binding{0}
{ 
	setPNext(nullptr);
	setFlags(0);
	setUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setQueueFamilyIndexCount(0);
	setPQueueFamilyIndices(nullptr);
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VertexBuffer::setFirstBinding(uint32_t first_binding) {
	_first_binding = first_binding;
}

uint32_t VertexBuffer::getFirstBinding() {
	return _first_binding;
}