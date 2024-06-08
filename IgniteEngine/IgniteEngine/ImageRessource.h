#pragma once

#include "Ressource.h"
#include "Pixels.h"

class ImageRessource: public Ressource
{
protected:
	VkImage _image{ nullptr };
	VkImageView _image_view{ nullptr };

	VkImageCreateInfo _image_info{};
	VkImageViewCreateInfo _image_view_info{};

public:

	virtual void createImage() = 0;
	virtual void bind() = 0;
	virtual void createImageView() = 0;

	virtual void create() = 0;

	virtual void destroyImage() = 0;
	virtual void destroyImageView() = 0;
	virtual void destroy() = 0;

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

	VkFormat getImageFormat();
	uint32_t getImageExtentWidth() const;
	uint32_t getImageExtentHeight() const;
	VkImageLayout getImageLayout() const;

	const VkImage& getImage() const;
	void setImage(VkImage image);
	const VkImageView& getImageView() const;

	virtual void flushPixels(Pixels& pixels) = 0;
	virtual void changeLayout(VkImageLayout new_layout,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	) = 0;
};