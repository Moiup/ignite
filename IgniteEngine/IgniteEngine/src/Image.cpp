#include "Image.h"

Image::Image()
{
	_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
}

void Image::setQueue(Queue* queue) {
	Ressource::setQueue(queue);
	_staging_buffer.setQueue(queue);
}

void Image::createImage() {
	VkResult vk_result = vkCreateImage(
		_queue->getDevice()->getDevice(),
		&_image_info,
		nullptr,
		&_image
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the image.");
	}
}

void Image::bind(){
	vkBindImageMemory(
		_queue->getDevice()->getDevice(),
		_image,
		_memory,
		0
	);
}

void Image::createImageView() {
	_image_view_info.image = _image;

	VkResult vk_result = vkCreateImageView(
		_queue->getDevice()->getDevice(),
		&_image_view_info,
		nullptr,
		&_image_view
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating image view");
	}
}

void Image::createStagingBuffer() {
	// Creating the staging buffer
	_staging_buffer.setQueue(_queue);
	_staging_buffer.setSize(
		_image_info.extent.width * _image_info.extent.height * _n * sizeof(uint8_t)
	);
	_staging_buffer.create();
}

void Image::create() {
	createImage();
	allocateMemory();
	bind();
	createImageView();
	createStagingBuffer();
}

void Image::destroyImage() {
	vkDestroyImage(
		_queue->getDevice()->getDevice(),
		_image,
		nullptr
	);
}

void Image::destroyImageView() {
	vkDestroyImageView(
		_queue->getDevice()->getDevice(),
		_image_view,
		nullptr
	);
}

void Image::destroy() {
	freeMemory();
	destroyImage();
	destroyImageView();
}

void Image::getMemoryRequirements() {
	vkGetImageMemoryRequirements(
		_queue->getDevice()->getDevice(),
		_image,
		&_memory_req
	);
}

void Image::copy(
	Buffer& buffer,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	VkImageLayout image_layout = _image_info.initialLayout;

	changeLayout(
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	// To do for each mip level
	// (To start, we consider only the original level -> 0)
	VkBufferImageCopy image_copy{};
	image_copy.bufferOffset = 0;
	image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.imageSubresource.mipLevel = 0;
	image_copy.imageSubresource.baseArrayLayer = 0;
	image_copy.imageSubresource.layerCount = 1;
	image_copy.imageExtent.width = getImageExtentWidth();
	image_copy.imageExtent.height = getImageExtentHeight();
	image_copy.imageExtent.depth = 1;

	CommandBuffer cmd_buff = _queue->allocateCommandBuffer();
	cmd_buff.begin();

	cmd_buff.copyBufferToImage(
		_staging_buffer.getBuffer(),
		Image::getImage(),
		Image::getImageLayout(),
		1,
		&image_copy
	);

	cmd_buff.end();

	changeLayout(
		image_layout,
		_stage_access_info.access_mask,
		dst_access_mask,
		_stage_access_info.stage_mask,
		dst_stage_mask
	);
}

void Image::copy(
	Image& src_img,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	VkImageLayout image_layout = _image_info.initialLayout;

	VkImageLayout src_image_layout_initial = src_img.getImageLayout();
	Queue* src_img_queue = src_img.getQueue();
	src_img.setQueue(_queue);
	src_img.changeLayout(
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_READ_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	changeLayout(
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	// To do for each mip level
	// (To start, we consider only the original level -> 0)
	VkImageCopy image_copy{};
	image_copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.srcSubresource.mipLevel = 0;
	image_copy.srcSubresource.baseArrayLayer = 0;
	image_copy.srcSubresource.layerCount = 1;
	image_copy.srcOffset = { 0 , 0 , 0 };
	image_copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.dstSubresource.mipLevel = 0;
	image_copy.dstSubresource.baseArrayLayer = 0;
	image_copy.dstSubresource.layerCount = 1;
	image_copy.dstOffset = { 0, 0, 0 };
	image_copy.extent.width = getImageExtentWidth();
	image_copy.extent.height = getImageExtentHeight();
	image_copy.extent.depth = 1;

	CommandBuffer cmd_buff = _queue->allocateCommandBuffer();
	cmd_buff.begin();

	cmd_buff.copyImageToImage(
		src_img.getImage(),
		src_img.getImageLayout(),
		Image::getImage(),
		Image::getImageLayout(),
		1,
		&image_copy
	);

	cmd_buff.end();

	src_img.changeLayout(
		src_image_layout_initial,
		src_img._stage_access_info.access_mask,
		dst_access_mask,
		src_img._stage_access_info.stage_mask,
		dst_stage_mask
	);

	changeLayout(
		image_layout,
		_stage_access_info.access_mask,
		dst_access_mask,
		_stage_access_info.stage_mask,
		dst_stage_mask
	);

	src_img.setQueue(src_img_queue);
}

void Image::update(Pixels& pixels) {
	// Copying the actual texture data into the staging buffer
	_staging_buffer.setValues(pixels.getPixels().data());

	// Copying the data from the buffer to the image
	copy(
		_staging_buffer,
		0,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	//_queue->submit();
	//_queue->wait();
}

void Image::flushToStaging() {
	_staging_buffer.copy(*this);
}

void Image::flushPixels(Pixels& pixels) {
	flushToStaging();
	_queue->submit();
	_queue->wait();
	_staging_buffer.getValues(pixels.getPixels().data());
}

void Image::changeLayout(VkImageLayout new_layout,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	CommandBuffer cmd_buf = _queue->allocateCommandBuffer();
	cmd_buf.begin();

	// Preparing the transfer with the image memory barrier
	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier{};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.image = Image::getImage();
	image_memory_barrier.subresourceRange = subresource_range;
	image_memory_barrier.srcAccessMask = src_access_mask;
	image_memory_barrier.dstAccessMask = dst_access_mask;
	image_memory_barrier.oldLayout = _image_info.initialLayout;
	image_memory_barrier.newLayout = new_layout;

	cmd_buf.pipelineBarrier(
		src_stage_mask,
		dst_stage_mask,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier
	);

	cmd_buf.end();

	_stage_access_info.access_mask = dst_access_mask;
	_stage_access_info.stage_mask = dst_stage_mask;

	_image_info.initialLayout = new_layout;
}