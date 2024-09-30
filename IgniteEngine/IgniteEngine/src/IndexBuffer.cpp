#include "IndexBuffer.h"

IndexBuffer::IndexBuffer() :
	Buffer::Buffer(),
	_index_type{ VK_INDEX_TYPE_UINT32 },
	_nb_elem{0}
{
	setPNext(nullptr);
	setFlags(0);
	setUsage(VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setQueueFamilyIndexCount(0);
	setPQueueFamilyIndices(nullptr);
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void IndexBuffer::setIndexType(VkIndexType index_type) {
	_index_type = index_type;
}

VkIndexType IndexBuffer::getIndexType() {
	return _index_type;
}

void IndexBuffer::setNbElem(uint32_t nb_elem) {
	_nb_elem = nb_elem;
}
uint32_t IndexBuffer::getNbElem() {
	return _nb_elem;
}