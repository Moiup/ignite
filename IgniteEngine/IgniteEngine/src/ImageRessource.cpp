#include "ImageRessource.h"

void ImageRessource::setImagePNext(const void* p_next) {
	_image_info.pNext = p_next;
}

void ImageRessource::setImageFlags(VkImageCreateFlags flags) {
	_image_info.flags = flags;
}

void ImageRessource::setImageImageType(VkImageType image_type) {
	_image_info.imageType = image_type;
}

void ImageRessource::setImageFormat(VkFormat format) {
	_image_info.format = format;
}

void ImageRessource::setImageExtent(VkExtent3D extent) {
	_image_info.extent = extent;
}

void ImageRessource::setImageExtent(uint32_t width, uint32_t height, uint32_t depth) {
	_image_info.extent.width = width;
	_image_info.extent.height = height;
	_image_info.extent.depth = depth;
}

void ImageRessource::setImageMipLevels(uint32_t mip_level) {
	_image_info.mipLevels = mip_level;
}

void ImageRessource::setImageArrayLayers(uint32_t array_layers) {
	_image_info.arrayLayers = array_layers;
}

void ImageRessource::setImageSamples(VkSampleCountFlagBits samples) {
	_image_info.samples = samples;
}

void ImageRessource::setImageTiling(VkImageTiling tiling) {
	_image_info.tiling = tiling;
}

void ImageRessource::setImageUsage(VkImageUsageFlags usage) {
	_image_info.usage = usage;
}

void ImageRessource::setImageSharingMode(VkSharingMode sharing_mode) {
	_image_info.sharingMode = sharing_mode;
}

void ImageRessource::setImageQueueFamilyIndexCount(uint32_t queue_family_index_count) {
	_image_info.queueFamilyIndexCount = queue_family_index_count;
}

void ImageRessource::setImagePQeueueFamilyIndices(const uint32_t* p_queue_family_indices) {
	_image_info.pQueueFamilyIndices = p_queue_family_indices;
}

void ImageRessource::setImageInitialLayout(VkImageLayout initial_layout) {
	_image_info.initialLayout = initial_layout;
}

void ImageRessource::setImageViewPNext(const void* p_next) {
	_image_view_info.pNext = p_next;
}

void ImageRessource::setImageViewFlags(VkImageViewCreateFlags flags) {
	_image_view_info.flags = flags;
}

void ImageRessource::setImageViewViewType(VkImageViewType view_type) {
	_image_view_info.viewType = view_type;
}

void ImageRessource::setImageViewFormat(VkFormat format) {
	_image_view_info.format = format;
}

void ImageRessource::setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle b, VkComponentSwizzle g, VkComponentSwizzle a) {
	_image_view_info.components.r = r;
	_image_view_info.components.g = g;
	_image_view_info.components.b = b;
	_image_view_info.components.a = a;
}

void ImageRessource::setImageViewSurbresourceRange(VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count) {
	_image_view_info.subresourceRange.aspectMask = aspect_mask;
	_image_view_info.subresourceRange.baseMipLevel = base_mip_level;
	_image_view_info.subresourceRange.levelCount = level_count;
	_image_view_info.subresourceRange.baseArrayLayer = base_array_layer;
	_image_view_info.subresourceRange.layerCount = layer_count;
}

void ImageRessource::setImageViewInfo(VkImageViewCreateInfo info) {
	_image_view_info = info;
}

VkFormat ImageRessource::getImageFormat() {
	return _image_info.format;
}

uint32_t ImageRessource::getImageExtentWidth() const {
	return _image_info.extent.width;
}

uint32_t ImageRessource::getImageExtentHeight() const {
	return _image_info.extent.height;
}

VkImageLayout ImageRessource::getImageLayout() const {
	return _image_info.initialLayout;
}

const VkImage& ImageRessource::getImage() const {
	return _image;
}

void ImageRessource::setImage(VkImage image) {
	_image = image;
}

const VkImageView& ImageRessource::getImageView() const {
	return _image_view;
}