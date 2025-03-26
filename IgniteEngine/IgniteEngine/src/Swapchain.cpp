#include "Swapchain.h"

Swapchain::Swapchain() :
	_swapchain{},
	_info{}
{
	_nb_shared = new int32_t(1);

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
	//_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
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

Swapchain::Swapchain(
	Device& device,
	PhysicalDevice& gpu,
	const std::vector<uint32_t>& queue_family_index,
	WindowSurface& window,
	uint32_t nb_frame,
	uint32_t width,
	uint32_t height
) :
	Swapchain::Swapchain()
{
	setDevice(&device);
		
	setWidthHeight(width, height);
	setMinImageCount(nb_frame);
	//setQueueFamilyIndices({ queue.getFamilyIndex() });
	setQueueFamilyIndices(queue_family_index);
	setSurface(window.getSurface());

	std::vector<VkSurfaceFormatKHR> surface_formats = window.getSurfaceFormats(gpu);
	std::vector<VkFormat> accepted_format = {
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_FORMAT_B8G8R8A8_SRGB
	};

	VkFormat found_format = { VK_FORMAT_UNDEFINED };
	VkColorSpaceKHR color_space{};

	for (const VkFormat& af : accepted_format) {
		for (const VkSurfaceFormatKHR& sf : surface_formats) {
			if (sf.format == af) {
				found_format = sf.format;
				color_space = sf.colorSpace;
				break;
			}
		}

		if (found_format != VK_FORMAT_UNDEFINED) {
			break;
		}
	}

	if (found_format == VK_FORMAT_UNDEFINED) {
		throw std::runtime_error("Error: no proper format found!");
	}

	setImageFormat(found_format);

	createSwapchain();
	gettingImages();
}

Swapchain::Swapchain(const Swapchain& swapchain) {
	*this = swapchain;
}

Swapchain::Swapchain(Swapchain&& swapchain) {
	*this = std::move(swapchain);
}

Swapchain& Swapchain::operator=(const Swapchain& swapchain) {
	destroy();
	_device = swapchain._device;
	_swapchain = swapchain._swapchain;
	_info = swapchain._info;
	_image_count = swapchain._image_count;
	_images = swapchain._images;
	_image_view_info = swapchain._image_view_info;

	_nb_shared = swapchain._nb_shared;
	*_nb_shared += 1;

	return *this;
}

Swapchain& Swapchain::operator=(Swapchain&& swapchain) {
	destroy();
	_device = std::move(swapchain)._device;
	swapchain._device = nullptr;
	_swapchain = std::move(swapchain)._swapchain;
	swapchain._swapchain = nullptr;
	_info = std::move(swapchain)._info;
	_image_count = std::move(swapchain)._image_count;
	_images = std::move(swapchain._images);
	_image_view_info = std::move(swapchain)._image_view_info;

	_nb_shared = std::move(swapchain)._nb_shared;
	swapchain._nb_shared = nullptr;

	return *this;
}

Swapchain::~Swapchain() {
	destroy();
}

void Swapchain::setDevice(Device* device) {
	_device = device;
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

void Swapchain::setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a) {
	_image_view_info.components.r = r;
	_image_view_info.components.g = g;
	_image_view_info.components.b = b;
	_image_view_info.components.a = a;
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

Image& Swapchain::getCurrentImage() {
	return _images[_current_image_i];
}

const uint32_t Swapchain::getCurrentImageIndex() const {
	return _current_image_i;
}

const uint32_t& Swapchain::acquireNextImage(
	uint64_t timeout,
	VkSemaphore semaphore,
	VkFence fence
) {
	uint32_t p_image_index{0};
	VkResult vk_result = vkAcquireNextImageKHR(
		_device->getDevice(),
		_swapchain,
		timeout,
		semaphore,
		fence,
		&p_image_index
	);

	if (vk_result != VK_SUCCESS) {
		std::string res = string_VkResult(vk_result);
		throw std::runtime_error("Swapchain::acquireNextImageKHR: Error getting next image: " + res);
	}
	_current_image_i = p_image_index;

	return _current_image_i;
}

const VkSwapchainCreateInfoKHR& Swapchain::getSwapchainInfo() const {
	return _info;
}

const VkSwapchainKHR& Swapchain::getSwapchain() const {
	return _swapchain;
}

void Swapchain::destroy() {
	if (!_nb_shared) {
		return;
	}
	*_nb_shared -= 1;
	if (*_nb_shared) {
		return;
	}
	delete _nb_shared;

	destroySwapchain();
}

void Swapchain::createSwapchain() {
	if (_info.queueFamilyIndexCount == 1) {
		_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}

	VkResult vk_result = vkCreateSwapchainKHR(
		_device->getDevice(),
		&_info,
		nullptr,
		&_swapchain
	);

	if (vk_result != VK_SUCCESS) {
		std::cout << vk_result << " " << string_VkResult(vk_result) << std::endl;
		throw std::runtime_error("Error: failed to create swapchain!");
	}
}

void Swapchain::gettingImages(){
	VkResult vk_result = vkGetSwapchainImagesKHR(
		_device->getDevice(),
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
		_device->getDevice(),
		_swapchain,
		&_image_count,
		imgs.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed getting the images!");
	}

	_image_view_info.format = _info.imageFormat;
	for (uint32_t i = 0; i < _image_count; i++) {
		_images[i] = Image(
			_device,
			imgs[i],
			_info.imageExtent.width,
			_info.imageExtent.height,
			1,
			_image_view_info
		);
	}
}

void Swapchain::createImagesViews(){	
	//_image_view_info.format = _info.imageFormat;

	//for (uint32_t i = 0; i < _image_count; i++) {
	//	//_image_view_info.image = _images[i].getImage();

	//	_images[i].setImageViewInfo(_image_view_info);
	//	_images[i].createImageView();
	//}
}

void Swapchain::destroySwapchain() {
	if (!_swapchain) {
		return;
	}

	vkDestroySwapchainKHR(
		_device->getDevice(),
		_swapchain,
		nullptr
	);

	for (uint32_t i = 0; i < _image_count; i++) {
		_images[i]._image = nullptr;
	}
}

void Swapchain::destroyImageViews() {
	
}