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
	allocateMemory();
	bind();
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
	copy = map();
	memcpy(copy, values, _size);
	unmap();
}

void Buffer::bind() {
	VkResult vk_result = vkBindBufferMemory(
		_logical_device->getDevice(),
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
	if (size > _capacity) {
		_capacity = size;
		_buffer_info.size = _capacity;
	}
	_size = size;
}

void Buffer::setCapacity(VkDeviceSize capacity) {
	if (capacity < _buffer_info.size) {
		std::cerr << "Buffer::setCapacity: Error! Capacity must be superior to size." << std::endl;
		return;
	}

	_capacity = capacity;
	_buffer_info.size = _capacity;
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

Pointer<uint8_t> Buffer::getValues() {
	if (_size == 0) {
		return nullptr;
	}

	void* copy{};
	uint8_t* v = new uint8_t[_size];
	copy = map();
	memcpy(v, copy, _size);
	unmap();

	return Pointer<uint8_t>(v);
}

void Buffer::copy(const Buffer& buffer) {

}

void Buffer::copySync(const Buffer& buffer) {

}

void* Buffer::map() {
	if (_size == 0) {
		return nullptr;
	}
	
	void* ptr{ nullptr };
	vkMapMemory(
		_logical_device->getDevice(),
		_memory,
		0,
		_size,
		0,
		&ptr
	);

	return ptr;
}

void Buffer::unmap() {
	vkUnmapMemory(
		_logical_device->getDevice(),
		_memory
	);
}

void Buffer::createBuffer() {
	VkResult vk_result = vkCreateBuffer(
		_logical_device->getDevice(),
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
		_logical_device->getDevice(),
		_buffer,
		nullptr
	);
}

void Buffer::getMemoryRequirements() {
	vkGetBufferMemoryRequirements(
		_logical_device->getDevice(),
		_buffer,
		&_memory_req
	);
}