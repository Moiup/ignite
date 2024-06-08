#pragma once

#include "ImageRessource.h"
#include "StagingBuffer.h"
#include <vector>

class Image: public ImageRessource
{
protected:
	StagingBuffer _staging_buffer{};

public:
	Image();

	void createImage();
	void bind();
	void createImageView();

	void create();
	
	void destroyImage();
	void destroyImageView();
	void destroy();

	//using  ImageRessource::setImagePNext;
	//void setImageFlags(VkImageCreateFlags flags);
	//void setImageImageType(VkImageType image_type);
	//void setImageFormat(VkFormat format);
	//void setImageExtent(VkExtent3D extent);
	//void setImageExtent(uint32_t width, uint32_t height, uint32_t depth);
	//void setImageMipLevels(uint32_t mip_level);
	//void setImageArrayLayers(uint32_t array_layers);
	//void setImageSamples(VkSampleCountFlagBits samples);
	//void setImageTiling(VkImageTiling tiling);
	//void setImageUsage(VkImageUsageFlags usage);
	//void setImageSharingMode(VkSharingMode sharing_mode);
	//void setImageQueueFamilyIndexCount(uint32_t queue_family_index_count);
	//void setImagePQeueueFamilyIndices(const uint32_t* p_queue_family_indices);
	//void setImageInitialLayout(VkImageLayout initial_layout);

	//void setImageViewPNext(const void* p_next);
	//void setImageViewFlags(VkImageViewCreateFlags flags);
	//void setImageViewViewType(VkImageViewType view_type);
	//void setImageViewFormat(VkFormat format);
	//void setImageViewComponents(VkComponentSwizzle r, VkComponentSwizzle b, VkComponentSwizzle g, VkComponentSwizzle a);
	//void setImageViewSurbresourceRange(VkImageAspectFlags aspect_mask, uint32_t base_mip_level, uint32_t level_count, uint32_t base_array_layer, uint32_t layer_count);
	//void setImageViewInfo(VkImageViewCreateInfo info);

	//VkFormat getImageFormat();
	//uint32_t getImageExtentWidth() const;
	//uint32_t getImageExtentHeight() const;
	//VkImageLayout getImageLayout() const;

	//const VkImage& getImage() const;
	//void setImage(VkImage image);
	//const VkImageView& getImageView() const;

	// Copy buffer to image
	void copy(
		Buffer& buffer,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	void flushToStaging();

	// It submit the queue and synchronize the CPU
	void flushPixels(Pixels& pixels);

	void changeLayout(VkImageLayout new_layout,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);


protected:
	void getMemoryRequirements();
};

