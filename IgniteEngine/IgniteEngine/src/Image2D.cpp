#include "Image2D.h"

Image2D::Image2D(
	Device* device,
	uint32_t width,
	uint32_t height,
	IGEImgFormat format,
	VkImageUsageFlags usage,
	uint32_t* queue_family_indices,
	uint32_t queue_family_indices_count,
	ComponentSwizzle swizzle,
	VkImageAspectFlags aspect_mask,
	uint32_t base_mip_level,
	uint32_t level_count,
	uint32_t base_array_layer,
	uint32_t layer_count
) :
	Image::Image(
		device,
		width,
		height,
		1,
		format,
		usage,
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_VIEW_TYPE_2D,
		queue_family_indices,
		queue_family_indices_count,
		swizzle,
		aspect_mask,
		base_mip_level,
		level_count,
		base_array_layer,
		layer_count
	)
{
	;
}

Image2D::Image2D(
	Device* device,
	uint32_t width,
	uint32_t height,
	IGEImgFormat format,
	VkImageUsageFlags usage
) :
	Image::Image(
		device,
		width,
		height,
		1,
		format,
		usage,
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_VIEW_TYPE_2D
	)
{
	;
}

Image2D::Image2D(
	Device* device,
	VkImage vk_img,
	uint32_t width,
	uint32_t height,
	VkImageViewCreateInfo view_info
) :
	Image::Image(
		device,
		vk_img,
		width,
		height,
		1,
		view_info
	)
{
	;
}