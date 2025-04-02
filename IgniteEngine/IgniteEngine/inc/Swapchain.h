#pragma once

#include "WindowSurface.h"
#include "Image.h"
#include "Image2D.h"
#include "Device.h"

class Swapchain
{
private:
	Device* _device;
	VkSwapchainKHR _swapchain{ nullptr };
	VkSwapchainCreateInfoKHR _info;

	uint32_t _image_count;
	//std::vector<VkImage> _images;
	//std::vector<VkImageView> _image_views;
	std::vector<Image2D> _images;
	VkImageViewCreateInfo _image_view_info;
	uint32_t _current_image_i{0};

	int32_t* _nb_shared{nullptr};

public:
	Swapchain();
	Swapchain(
		Device& device,
		PhysicalDevice& gpu,
		const std::vector<uint32_t>& queue_family_index,
		WindowSurface& window,
		uint32_t nb_frame,
		uint32_t width,
		uint32_t height
	);
	Swapchain(const Swapchain& swapchain);
	Swapchain(Swapchain&& swapchain);
	~Swapchain();

	Swapchain& operator=(const Swapchain& swapchain);
	Swapchain& operator=(Swapchain&& swapchain);

	void setDevice(Device* _device);

	void setPNext(void *p_next);
	void setFlags(VkSwapchainCreateFlagsKHR flags);
	void setSurface(VkSurfaceKHR surface);
	void setMinImageCount(uint32_t min_image_count);
	void setImageFormat(VkFormat image_format);
	void setImageColorSpace(VkColorSpaceKHR image_color_space);
	void setWidthHeight(uint32_t width, uint32_t height);
	void setImageArrayLayers(uint32_t image_array_layers);
	void setImageUsage(VkImageUsageFlags image_usage);
	void setQueueFamilyIndices(const std::vector<uint32_t>& queue_family_indices);
	void setPreTransform(VkSurfaceTransformFlagBitsKHR pre_transform);
	void setCompositeAlpha(VkCompositeAlphaFlagBitsKHR composite_alpha);
	void setPresentMode(VkPresentModeKHR present_mode);
	void setClipped(VkBool32 clipped);
	void setOldSwapchain(VkSwapchainKHR old_swapchain);

	void setImageViewPNext(const void* p_next);
	void setImageViewFlags(VkImageViewCreateFlags flags);
	void setImageViewViewType(VkImageViewType view_type);
	void setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a);
	void setImageViewSurbresourceRange(VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count);

	std::vector<Image2D>& getImages();
	Image2D& getCurrentImage();
	const uint32_t getCurrentImageIndex() const;

	const uint32_t& acquireNextImage(
		uint64_t timeout, 
		VkSemaphore semaphore,
		VkFence fence
		);

	const VkSwapchainCreateInfoKHR& getSwapchainInfo() const;
	const VkSwapchainKHR& getSwapchain() const;

	void destroy();

private:
	void createSwapchain();
	void gettingImages();
	void createImagesViews();

	void destroySwapchain();
	void destroyImageViews();
};

