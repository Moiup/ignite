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
		_queue->getDevice()->getDevice(),
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

uint32_t Buffer::getSize() {
	return _size;
}

uint32_t Buffer::getCapacity() {
	return _capacity;
}

void* Buffer::map() {
	if (_size == 0) {
		return nullptr;
	}
	
	void* ptr{ nullptr };
	vkMapMemory(
		_queue->getDevice()->getDevice(),
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
		_queue->getDevice()->getDevice(),
		_memory
	);
}

void Buffer::copy(Image& img) {
	CommandBuffer cmd_buf = _queue->allocateCommandBuffer();

	cmd_buf.begin();

	// Change image layout to transfer
	VkImageLayout initial_layout = img.getImageLayout();
	img.changeLayout(
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
	);

	// copy
	VkImageSubresourceLayers img_subresource{};
	img_subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	img_subresource.mipLevel = 0;
	img_subresource.baseArrayLayer = 0;
	img_subresource.layerCount = 1;

	VkBufferImageCopy img_copy{};
	img_copy.bufferOffset = 0;
	img_copy.bufferRowLength = 0;
	img_copy.bufferImageHeight = 0;
	img_copy.imageSubresource = img_subresource;
	img_copy.imageOffset = { 0, 0, 0 };
	img_copy.imageExtent.width = img.getImageExtentWidth();
	img_copy.imageExtent.height = img.getImageExtentHeight();
	img_copy.imageExtent.depth = 1;

	cmd_buf.copyImageToBuffer(
		img.getImage(),
		img.getImageLayout(),
		_buffer,
		1,
		&img_copy
	);

	// pipeline barrier
	cmd_buf.pipelineBarrier(
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		0, nullptr
	);

	// Changing image layout back
	img.changeLayout(initial_layout);

	cmd_buf.end();

	//_stage_access_info.access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	_stage_access_info.stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
}

void Buffer::createBuffer() {
	VkResult vk_result = vkCreateBuffer(
		_queue->getDevice()->getDevice(),
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
		_queue->getDevice()->getDevice(),
		_buffer,
		nullptr
	);
}

void Buffer::getMemoryRequirements() {
	vkGetBufferMemoryRequirements(
		_queue->getDevice()->getDevice(),
		_buffer,
		&_memory_req
	);
}