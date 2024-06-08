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

	void update(Pixels& pixels);

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

