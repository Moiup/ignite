#include "Swapchain.h"

Swapchain::Swapchain() :
	_swapchain{},
	_info{},
	_created{false}
{

	// -- Swapchain infos -- //
	_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	_info.pNext = nullptr;
	_info.flags = 0;
	// surface must be set
	// min image count must be set
	_info.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	// Image extent (width and height) must be set
	_info.imageArrayLayers = 1;
	_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		| VK_IMAGE_USAGE_TRANSFER_DST_BIT
		| VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	// queue family index count must be set
	// queue family indices must be set
	_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	_info.clipped = VK_TRUE;
	_info.oldSwapchain = VK_NULL_HANDLE;

	// -- Image view infos -- //
	_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	_image_view_info.pNext = nullptr;
	_image_view_info.flags = 0;
	_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	_image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	_image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	_image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	_image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	_image_view_info.subresourceRange.baseMipLevel = 0;
	_image_view_info.subresourceRange.levelCount = 1;
	_image_view_info.subresourceRange.baseArrayLayer = 0;
	_image_view_info.subresourceRange.layerCount = 1;
}

void Swapchain::setQueue(Queue* queue) {
	_queue = queue;
}

void Swapchain::setPNext(void* p_next) {
	_info.pNext = p_next;
}

void Swapchain::setFlags(VkSwapchainCreateFlagsKHR flags) {
	_info.flags = flags;
}

void Swapchain::setSurface(VkSurfaceKHR surface) {
	_info.surface = surface;
}

void Swapchain::setMinImageCount(uint32_t min_image_count) {
	_info.minImageCount = min_image_count;
}

void Swapchain::setImageFormat(VkFormat image_format) {
	_info.imageFormat = image_format;
}

void Swapchain::setImageColorSpace(VkColorSpaceKHR image_color_space) {
	_info.imageColorSpace = image_color_space;
}

void Swapchain::setWidthHeight(uint32_t width, uint32_t height) {
	_info.imageExtent.width = width;
	_info.imageExtent.height = height;
}

void Swapchain::setImageArrayLayers(uint32_t image_array_layers) {
	_info.imageArrayLayers = image_array_layers;
}

void Swapchain::setImageUsage(VkImageUsageFlags image_usage){
	_info.imageUsage = image_usage;
}

void Swapchain::setQueueFamilyIndices(const std::vector<uint32_t>& queue_family_indices) {
	_info.queueFamilyIndexCount = queue_family_indices.size();
	_info.pQueueFamilyIndices = queue_family_indices.data();
}

void Swapchain::setPreTransform(VkSurfaceTransformFlagBitsKHR pre_transform) {
	_info.preTransform = pre_transform;
}

void Swapchain::setCompositeAlpha(VkCompositeAlphaFlagBitsKHR composite_alpha) {
	_info.compositeAlpha = composite_alpha;
}

void Swapchain::setPresentMode(VkPresentModeKHR present_mode) {
	_info.presentMode = present_mode;
}

void Swapchain::setClipped(VkBool32 clipped) {
	_info.clipped = clipped;
}

void Swapchain::setOldSwapchain(VkSwapchainKHR old_swapchain) {
	_info.oldSwapchain = old_swapchain;
}

void Swapchain::setImageViewPNext(const void* p_next) {
	_image_view_info.pNext = p_next;
}

void Swapchain::setImageViewFlags(VkImageViewCreateFlags flags) {
	_image_view_info.flags = flags;
}

void Swapchain::setImageViewViewType(VkImageViewType view_type) {
	_image_view_info.viewType = view_type;
}

void Swapchain::setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle b, VkComponentSwizzle g, VkComponentSwizzle a) {
	_image_view_info.components.r = r;
	_image_view_info.components.r = g;
	_image_view_info.components.r = b;
	_image_view_info.components.r = a;
}

void Swapchain::setImageViewSurbresourceRange(VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count) {
	_image_view_info.subresourceRange.aspectMask = aspect_mask;
	_image_view_info.subresourceRange.baseMipLevel = base_mip_level;
	_image_view_info.subresourceRange.levelCount = level_count;
	_image_view_info.subresourceRange.baseArrayLayer = base_array_layer;
	_image_view_info.subresourceRange.layerCount = layer_count;
}

std::vector<Image>& Swapchain::getImages() {
	return _images;
}

const VkSwapchainCreateInfoKHR& Swapchain::getSwapchainInfo() const {
	return _info;
}

const VkSwapchainKHR& Swapchain::getSwapchain() const {
	return _swapchain;
}

void Swapchain::create() {
	createSwapchain();
	gettingImages();
	createImagesViews();

	_created = true;
}

void Swapchain::destroy() {
	if (!_created) {
		return;
	}
	destroySwapchain();
	destroyImageViews();
}

void Swapchain::createSwapchain() {
	if (_info.queueFamilyIndexCount == 1) {
		_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}

	VkResult vk_result = vkCreateSwapchainKHR(
		_queue->getDevice()->getDevice(),
		&_info,
		nullptr,
		&_swapchain
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create swapchain!");
	}
}

void Swapchain::gettingImages(){
	VkResult vk_result = vkGetSwapchainImagesKHR(
		_queue->getDevice()->getDevice(),
		_swapchain,
		&_image_count,
		nullptr
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed getting the images count!");
	}

	_images.resize(_image_count);
	std::vector<VkImage> imgs(_image_count);
	vk_result = vkGetSwapchainImagesKHR(
		_queue->getDevice()->getDevice(),
		_swapchain,
		&_image_count,
		imgs.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed getting the images!");
	}

	for (uint32_t i = 0; i < _image_count; i++) {
		_images[i].setQueue(_queue);
		_images[i].setImage(imgs[i]);
		_images[i].setImageExtent(
			_info.imageExtent.width,
			_info.imageExtent.height,
			1
		);
		_images[i].changeLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		_images[i].createStagingBuffer();
	}
}

void Swapchain::createImagesViews(){	
	_image_view_info.format = _info.imageFormat;

	for (uint32_t i = 0; i < _image_count; i++) {
		//_image_view_info.image = _images[i].getImage();

		_images[i].setImageViewInfo(_image_view_info);
		_images[i].createImageView();
	}
}

void Swapchain::destroySwapchain() {
	vkDestroySwapchainKHR(
		_queue->getDevice()->getDevice(),
		_swapchain,
		nullptr
	);
}

void Swapchain::destroyImageViews() {
	for (uint32_t i = 0; i < _image_count; i++) {
		_images[i].destroyImageView();
	}
}