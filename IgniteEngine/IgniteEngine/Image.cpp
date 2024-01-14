#include "Image.h"

Image::Image() :
	Ressource::Ressource(),
	_image{nullptr},
	_image_view{nullptr},
	_image_info{},
	_image_view_info{}
{
	_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
}

void Image::createImage() {
	VkResult vk_result = vkCreateImage(
		*_logical_device->getDevice(),
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
		*_logical_device->getDevice(),
		_image,
		_memory,
		0
	);
}

void Image::createImageView() {
	_image_view_info.image = _image;

	VkResult vk_result = vkCreateImageView(
		*_logical_device->getDevice(),
		&_image_view_info,
		nullptr,
		&_image_view
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating image view");
	}
}

void Image::destroyImage() {
	vkDestroyImage(*_logical_device->getDevice(), _image, nullptr);
}

void Image::destroyImageView() {
	vkDestroyImageView(*_logical_device->getDevice(), _image_view, nullptr);
}

void Image::setImagePNext(const void* p_next) {
	_image_info.pNext = p_next;
}

void Image::setImageFlags(VkImageCreateFlags flags) {
	_image_info.flags = flags;
}

void Image::setImageImageType(VkImageType image_type) {
	_image_info.imageType = image_type;
}

void Image::setImageFormat(VkFormat format) {
	_image_info.format = format;
}

void Image::setImageExtent(VkExtent3D extent) {
	_image_info.extent = extent;
}

void Image::setImageExtent(uint32_t width, uint32_t height, uint32_t depth) {
	_image_info.extent.width = width;
	_image_info.extent.height = height;
	_image_info.extent.depth = depth;
}

void Image::setImageMipLevels(uint32_t mip_level) {
	_image_info.mipLevels = mip_level;
}

void Image::setImageArrayLayers(uint32_t array_layers) {
	_image_info.arrayLayers = array_layers;
}

void Image::setImageSamples(VkSampleCountFlagBits samples) {
	_image_info.samples = samples;
}

void Image::setImageTiling(VkImageTiling tiling) {
	_image_info.tiling = tiling;
}

void Image::setImageUsage(VkImageUsageFlags usage) {
	_image_info.usage = usage;
}

void Image::setImageSharingMode(VkSharingMode sharing_mode) {
	_image_info.sharingMode = sharing_mode;
}

void Image::setImageQueueFamilyIndexCount(uint32_t queue_family_index_count) {
	_image_info.queueFamilyIndexCount = queue_family_index_count;
}

void Image::setImagePQeueueFamilyIndices(const uint32_t* p_queue_family_indices) {
	_image_info.pQueueFamilyIndices = p_queue_family_indices;
}

void Image::setImageInitialLayout(VkImageLayout initial_layout) {
	_image_info.initialLayout = initial_layout;
}

void Image::setImageViewPNext(const void* p_next) {
	_image_view_info.pNext = p_next;
}

void Image::setImageViewFlags(VkImageViewCreateFlags flags) {
	_image_view_info.flags = flags;
}

void Image::setImageViewViewType(VkImageViewType view_type) {
	_image_view_info.viewType = view_type;
}

void Image::setImageViewFormat(VkFormat format) {
	_image_view_info.format = format;
}

void Image::setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle b, VkComponentSwizzle g, VkComponentSwizzle a) {
	_image_view_info.components.r = r;
	_image_view_info.components.r = g;
	_image_view_info.components.r = b;
	_image_view_info.components.r = a;
}

void Image::setImageViewSurbresourceRange(VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count) {
	_image_view_info.subresourceRange.aspectMask = aspect_mask;
	_image_view_info.subresourceRange.baseMipLevel = base_mip_level;
	_image_view_info.subresourceRange.levelCount = level_count;
	_image_view_info.subresourceRange.baseArrayLayer = base_array_layer;
	_image_view_info.subresourceRange.layerCount = layer_count;
}

void Image::setImageViewInfo(VkImageViewCreateInfo info) {
	_image_view_info = info;
}

VkFormat Image::getImageFormat() {
	return _image_info.format;
}

uint32_t Image::getImageExtentWidth() const {
	return _image_info.extent.width;
}

uint32_t Image::getImageExtentHeight() const {
	return _image_info.extent.height;
}

const VkImage& Image::getImage() const {
	return _image;
}

void Image::setImage(VkImage image) {
	_image = image;
}

const VkImageView& Image::getImageView() const {
	return _image_view;
}

void Image::getMemoryRequirements() {
	vkGetImageMemoryRequirements(
		*_logical_device->getDevice(),
		_image,
		&_memory_req
	);
}
