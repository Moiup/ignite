#pragma once

#include "Device.h"
#include "Swapchain.h"
#include "DepthBuffer.h"
#include "ComputePipeline.h"
#include "GraphicsPipeline.h"

class CommandBuffer
{
	friend class CommandPool;

private:
	VkCommandBuffer _command_buffer{ nullptr };
	VkCommandBufferLevel _level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY };

public:
	CommandBuffer();
	CommandBuffer(
		VkCommandBuffer command_buffer,
		VkCommandBufferLevel level
	);
	CommandBuffer(const CommandBuffer& cmd_buf);
	//CommandBuffer(CommandBuffer&& cmd_buf);

	// CommandBuffer& operator=(const CommandBuffer& cmd_buf);

	void setLevel(VkCommandBufferLevel level);

	VkCommandBuffer getCommandBuffer();
	VkCommandBufferLevel getLevel();

	void begin();
	void begin(VkCommandBufferUsageFlags flags);
	void end();
	void reset();

	void beginRendering(VkRenderingInfoKHR& info);

	void beginRendering(
		VkClearColorValue clear_color,
		Image& image,
		DepthBuffer& depth_buffer,
		VkOffset2D offset,
		VkExtent2D extent
	);
	void beginRendering(
		VkClearColorValue clear_color,
		Image& image,
		DepthBuffer& depth_buffer,
		VkOffset2D offset = {0, 0}
	);

	void bindPipeline(VkPipelineBindPoint bind_point, VkPipeline pipeline);
	
	void bindPipeline(GraphicsPipeline& gp);
	void bindPipeline(ComputePipeline& cp);

	void setViewport(std::vector<VkViewport>& viewport_arr);
	void setViewport(VkViewport* p_viewport, uint32_t count);
	void setScissor(std::vector<VkRect2D>& scissor_arr);
	void setScissor(VkRect2D* p_scissor, uint32_t count);
	void setCullMode(VkCullModeFlags cull_mode);
	void setFrontFace(VkFrontFace front_face);
	void bindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout pipeline_layout, uint32_t first_set, uint32_t descriptor_count, const VkDescriptorSet* p_descriptor_sets,
		uint32_t dynamic_offset_count, const uint32_t* p_dynamic_offsets);
	void pushConstants(const VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, const void* p_values);
	void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type);
	void bindVertexBuffer(uint32_t first_binding, uint32_t binding_count, const VkBuffer* p_buffers, const VkDeviceSize* p_offsets);

	void drawIndexed(
		const uint32_t index_count,
		const uint32_t instance_count,
		const uint32_t first_index,
		const uint32_t vertex_offset,
		const uint32_t first_instance
	);
	void endRendering(Image& swapchain);

	void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);
	void dispatch(
		ComputePipeline& cp,
		uint32_t group_count_x,	
		uint32_t group_count_y,	
		uint32_t group_count_z	
	);

	void endRendering();

	void dynamicRenderingPipelineBarrier(
		Image& image,
		DepthBuffer& depth_buffer
	);
	void dynamicRenderingPipelineBarrierBack(Image& image);

	void pipelineBarrier(
		VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkDependencyFlags dependencyFlags,
		uint32_t memoryBarrierCount,
		const VkMemoryBarrier* pMemoryBarriers,
		uint32_t bufferMemoryBarrierCount,
		const VkBufferMemoryBarrier* pBufferMemoryBarriers,
		uint32_t imageMemoryBarrierCount,
		const VkImageMemoryBarrier* pImageMemoryBarriers
	);

	/**
	* Uses vkCmdPipelineBarrier2
	*/
	void pipelineBarrier(const VkDependencyInfo* p_dependency_info);
	void barrier(
		VkPipelineStageFlags2 src_stage_mask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		VkPipelineStageFlags2 dst_stage_mask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		VkAccessFlags2 src_access_mask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
		VkAccessFlags2 dst_access_mask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT
	);

	void copyBufferToBuffer(
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		uint32_t regionCount,
		const VkBufferCopy2* pRegions
	);

	void copyBufferToImage(
		VkBuffer srcBuffer,
		VkImage dstImage,
		VkImageLayout dstImageLayout,
		uint32_t regionCount,
		const VkBufferImageCopy* pRegions
	);

	void copyImageToBuffer(
		VkImage src_image,
		VkImageLayout src_image_layout,
		VkBuffer dst_buffer,
		uint32_t region_count,
		const VkBufferImageCopy* p_regions
	);

	void copyImageToBuffer(
		VkCopyImageToBufferInfo2* p_copy_image_to_buffer_info
	);

	void copyImageToImage(
		VkImage src_image,
		VkImageLayout src_image_layout,
		VkImage dst_image,
		VkImageLayout dst_image_layout,
		uint32_t region_count,
		const VkImageCopy* p_regions
	);

	template<IGEBufferUsage U1, IGEBufferUsage U2>
	void copy(Buffer<U1>& src, Buffer<U2>& dst);

	template<IGEBufferUsage U>
	void copy(Buffer<U>& src, Image& dst,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	template<IGEBufferUsage U>
	void copy(Image& src, Buffer<U>& dst,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	void copy(Image& src, Image& dst,
		VkExtent3D extent,
		VkOffset3D src_offset = { 0, 0, 0 },
		VkOffset3D dst_offset = { 0, 0, 0 },
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);
	
	void copy(Image& src, Image& dst,
		VkOffset3D src_offset = { 0, 0, 0 },
		VkOffset3D dst_offset = { 0, 0, 0 },
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	void changeLayout(
		Image& img,
		VkImageLayout new_layout,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

private:
	void bindPipeline(VkPipelineBindPoint bind_point, Pipeline& pp);
};

template<IGEBufferUsage U1, IGEBufferUsage U2>
void CommandBuffer::copy(Buffer<U1>& src, Buffer<U2>& dst) {
	VkBufferCopy2 copy_info{};
	copy_info.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
	copy_info.pNext = nullptr;
	copy_info.srcOffset = 0;
	copy_info.dstOffset = 0;
	copy_info.size = src.getSize();

	copyBufferToBuffer(
		src.getBuffer(),
		dst.getBuffer(),
		1,
		&copy_info
	);
}

template<IGEBufferUsage U>
void CommandBuffer::copy(Image& src, Buffer<U>& dst,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	VkImageLayout image_layout = src.getImageLayout();

	changeLayout(
		src,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_READ_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	VkBufferImageCopy2 region_info{};
	region_info.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
	region_info.pNext = nullptr;
	region_info.bufferOffset = 0;
	region_info.bufferRowLength = 0;
	region_info.bufferImageHeight = 0;
	region_info.imageSubresource.aspectMask = src.aspectMask();
	region_info.imageSubresource.mipLevel = 0;
	region_info.imageSubresource.baseArrayLayer = 0;
	region_info.imageSubresource.layerCount = 1;
	region_info.imageOffset.x = 0;
	region_info.imageOffset.y = 0;
	region_info.imageOffset.z = 0;
	region_info.imageExtent.width = static_cast<uint32_t>(src.getWidth());
	region_info.imageExtent.height = static_cast<uint32_t>(src.getHeight());
	region_info.imageExtent.depth = 1;

	VkCopyImageToBufferInfo2 copy_info{};
	copy_info.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
	copy_info.pNext = nullptr;
	copy_info.srcImage = src.getImage();
	copy_info.srcImageLayout = src.getImageLayout();
	copy_info.dstBuffer = dst.getBuffer();
	copy_info.regionCount = 1;
	copy_info.pRegions = &region_info;

	copyImageToBuffer(&copy_info);

	changeLayout(
		src,
		image_layout,
		src.getStageAccessInfo().access_mask,
		dst_access_mask,
		src.getStageAccessInfo().stage_mask,
		dst_stage_mask
	);
}

template<IGEBufferUsage U>
void CommandBuffer::copy(Buffer<U>& src, Image& dst,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	VkImageLayout image_layout = dst.getImageLayout();

	changeLayout(
		dst,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);
	
	// To do for each mip level
	// (To start, we consider only the original level -> 0)
	VkBufferImageCopy image_copy{};
	image_copy.bufferOffset = 0;
	image_copy.imageSubresource.aspectMask = dst.aspectMask();
	image_copy.imageSubresource.mipLevel = 0;
	image_copy.imageSubresource.baseArrayLayer = 0;
	image_copy.imageSubresource.layerCount = 1;
	image_copy.imageExtent.width = dst.getWidth();
	image_copy.imageExtent.height = dst.getHeight();
	image_copy.imageExtent.depth = 1;

	copyBufferToImage(
		src.getBuffer(),
		dst.getImage(),
		dst.getImageLayout(),
		1,
		&image_copy
	);

	changeLayout(
		dst,
		image_layout,
		dst.getStageAccessInfo().access_mask,
		dst_access_mask,
		dst.getStageAccessInfo().stage_mask,
		dst_stage_mask
	);
}