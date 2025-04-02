#ifndef _IMAGE_2D_H_
#define _IMAGE_2D_H_

#include "Image.h"

class Image2D : public Image {
public:
	Image2D() = default;

	Image2D(
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
	);

	Image2D(
		Device* device,
		uint32_t width,
		uint32_t height,
		IGEImgFormat format,
		VkImageUsageFlags usage
	);

	Image2D(
		Device* device,
		VkImage vk_img,
		uint32_t width,
		uint32_t height,
		VkImageViewCreateInfo view_info
	);
};

#endif