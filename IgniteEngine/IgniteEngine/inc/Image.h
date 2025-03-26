#pragma once

#include "Ressource.h"
#include <vector>

class Queue;
class Swapchain;

struct ComponentSwizzle {
	VkComponentSwizzle r, g, b, a;
};

class Image: public Ressource
{
	friend Queue;
	friend Swapchain;
protected:
	static const uint8_t _n{ 4 };

	VkImage _image{ nullptr };
	VkImageView _image_view{ nullptr };

	VkImageCreateInfo _image_info{};
	VkImageViewCreateInfo _image_view_info{};


public:
	Image();

	Image(
		Device* device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		IGEImgFormat format,
		VkImageUsageFlags usage,
		VkImageType image_type,
		uint32_t* queue_family_indices,
		uint32_t queue_family_indices_count,
		ComponentSwizzle swizzle,
		VkImageAspectFlags aspect_mask,
		uint32_t base_mip_level,
		uint32_t level_count,
		uint32_t base_array_layer,
		uint32_t layer_count
	);

	Image(
		Device* device,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		IGEImgFormat format,
		VkImageUsageFlags usage,
		VkImageType image_type
	);

	Image(
		Device* device,
		VkImage vk_img,
		uint32_t width,
		uint32_t height,
		uint32_t depth,
		VkImageViewCreateInfo view_info
	);
	Image(const Image& img);
	Image(Image&& img);

	~Image();

	Image& operator=(const Image& img);
	Image& operator=(Image&& img);

protected:
	void createImage();
	void bind();
	void createImageView();

	void create();
	
	void destroyImage();
	void destroyImageView();
	void destroy();

	void setImagePNext(const void* p_next);
	void setImageFlags(VkImageCreateFlags flags);
	void setImageImageType(VkImageType image_type);
	void setImageFormat(VkFormat format);
	void setImageExtent(VkExtent3D extent);
	void setImageExtent(uint32_t width, uint32_t height, uint32_t depth);
	void setImageMipLevels(uint32_t mip_level);
	void setImageArrayLayers(uint32_t array_layers);
	void setImageSamples(VkSampleCountFlagBits samples);
	void setImageTiling(VkImageTiling tiling);
	void setImageUsage(VkImageUsageFlags usage);
	void setImageSharingMode(VkSharingMode sharing_mode);
	void setImageQueueFamilyIndexCount(uint32_t queue_family_index_count);
	void setImagePQeueueFamilyIndices(const uint32_t* p_queue_family_indices);
	void setImageInitialLayout(VkImageLayout initial_layout);

	void setImageViewPNext(const void* p_next);
	void setImageViewFlags(VkImageViewCreateFlags flags);
	void setImageViewViewType(VkImageViewType view_type);
	void setImageViewFormat(VkFormat format);
	void setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle b, VkComponentSwizzle g, VkComponentSwizzle a);
	void setImageViewSurbresourceRange(VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count);
	void setImageViewInfo(VkImageViewCreateInfo info);

	void setAspectMask(VkImageAspectFlags aspect_flag);

public:
	VkFormat getImageFormat();
	VkImageLayout getImageLayout() const;

	const VkImage& getImage() const;
	const VkImageView& getImageView() const;

	const uint32_t getWidth() const;
	const uint64_t getHeight() const;

	const VkImageAspectFlags aspectMask() const;

protected:
	void getMemoryRequirements();
};

