#include "Image.h"

Image::Image() :
	Ressource::Ressource()
{
	_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
}

Image::Image(
	Device* device,
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	IGEImgFormat format,
	VkImageUsageFlags usage,
	VkImageType image_type,
	VkImageViewType image_view_type,
	uint32_t* queue_family_indices,
	uint32_t queue_family_indices_count,
	ComponentSwizzle swizzle,
	VkImageAspectFlags aspect_mask,
	uint32_t base_mip_level,
	uint32_t level_count,
	uint32_t base_array_layer,
	uint32_t layer_count
) :
	Image()
{
	setDevice(device);
	setImageFormat(static_cast<VkFormat>(format));
	setImageMipLevels(1);
	setImageArrayLayers(1);
	setImageSamples(VK_SAMPLE_COUNT_1_BIT);
	setImageTiling(VK_IMAGE_TILING_OPTIMAL);
	setImageSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setImageInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
	setImageUsage(usage);
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	setImageExtent(width, height, depth);

	setImageViewFormat(static_cast<VkFormat>(format));
	setImageViewComponents(
		swizzle.r,
		swizzle.g,
		swizzle.b,
		swizzle.a
	);
	setImageViewSurbresourceRange(
		aspect_mask,
		base_mip_level, // base mip level
		level_count, // level count
		base_array_layer, // base array layer
		layer_count  // layer count
	);

	setImageImageType(image_type);
	setImageViewViewType(image_view_type);

	create();
}

Image::Image(
	Device* device,
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	IGEImgFormat format,
	VkImageUsageFlags usage,
	VkImageType image_type,
	VkImageViewType image_view_type
):
	Image(
		device,
		width,
		height,
		depth,
		format,
		usage,
		image_type,
		image_view_type,
		nullptr,
		0,
		{ VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY },
		VK_IMAGE_ASPECT_COLOR_BIT,
		0,
		1,
		0,
		1
	)
{
	;
}

Image::Image(
	Device* device,
	VkImage vk_img,
	uint32_t width,
	uint32_t height,
	uint32_t depth,
	VkImageViewCreateInfo view_info
) :
	Image()
{
	_device = device;
	_image = vk_img;
	setImageExtent(width, height, depth);
	_image_view_info = view_info;
	_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createImageView();
}

Image::Image(const Image& img) {
	*this = img;
}

Image::Image(Image&& img) {
	*this = std::move(img);
}

Image& Image::operator=(const Image& img) {
	destroy();
	Ressource::operator=(img);
	_image = img._image;
	_image_view = img._image_view;
	_image_info = img._image_info;
	_image_view_info = img._image_view_info;
	
	return *this;
}

Image& Image::operator=(Image&& img) {
	destroy();
	Ressource::operator=(std::move(img));
	_image = std::move(img)._image;
	img._image = nullptr;
	_image_view = std::move(img)._image_view;
	img._image_view = nullptr;
	_image_info = std::move(img)._image_info;
	_image_view_info = std::move(img)._image_view_info;

	return *this;
}

Image::~Image() {
	destroy();
}

void Image::createImage() {
	VkResult vk_result = vkCreateImage(
		_device->getDevice(),
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
		_device->getDevice(),
		_image,
		_memory,
		0
	);
}

void Image::createImageView() {
	_image_view_info.image = _image;

	VkResult vk_result = vkCreateImageView(
		_device->getDevice(),
		&_image_view_info,
		nullptr,
		&_image_view
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating image view");
	}
}


void Image::create() {
	createImage();
	allocateMemory();
	bind();
	createImageView();
}


void Image::destroyImage() {
	if (!_image) {
		return;
	}

	vkDestroyImage(
		_device->getDevice(),
		_image,
		nullptr
	);
	_image = nullptr;
}


void Image::destroyImageView() {
	if (!_image_view) {
		return;
	}
	vkDestroyImageView(
		_device->getDevice(),
		_image_view,
		nullptr
	);
	_image_view = nullptr;
}


void Image::destroy() {
	if (getNbShared() > 1) {
		return;
	}

	destroyImage();
	destroyImageView();
}


void Image::getMemoryRequirements() {
	vkGetImageMemoryRequirements(
		_device->getDevice(),
		_image,
		&_memory_req
	);
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
	_image_view_info.components.g = g;
	_image_view_info.components.b = b;
	_image_view_info.components.a = a;
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

void Image::setAspectMask(VkImageAspectFlags aspect_flag){
	_image_view_info.subresourceRange.aspectMask = aspect_flag;
}


VkFormat Image::getImageFormat() {
	return _image_info.format;
}

VkImageLayout Image::getImageLayout() const {
	return _image_info.initialLayout;
}

const VkImage& Image::getImage() const {
	return _image;
}


const VkImageView& Image::getImageView() const {
	return _image_view;
}

const uint64_t Image::getWidth() const {
	return _image_info.extent.width;
}


const uint64_t Image::getHeight() const {
	return _image_info.extent.height;
}

const uint64_t Image::getDepth() const {
	return _image_info.extent.depth;
}

const VkImageAspectFlags Image::aspectMask() const {
	return _image_view_info.subresourceRange.aspectMask;
}