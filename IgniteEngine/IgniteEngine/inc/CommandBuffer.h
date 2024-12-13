#pragma once

#include "Device.h"
#include "Swapchain.h"
#include "DepthBuffer.h"

class CommandBuffer
{
	friend class CommandPool;

private:
	VkCommandBuffer _command_buffer{ nullptr };
	VkCommandBufferLevel _level{ VK_COMMAND_BUFFER_LEVEL_PRIMARY };

public:
	CommandBuffer();

	CommandBuffer(const CommandBuffer& cmd_buf);
	//CommandBuffer(CommandBuffer&& cmd_buf);

	CommandBuffer& operator=(const CommandBuffer& cmd_buf);

	void setLevel(VkCommandBufferLevel level);

	VkCommandBuffer getCommandBuffer();
	VkCommandBufferLevel getLevel();


	void begin();
	void begin(VkCommandBufferUsageFlags flags);
	void end();
	void reset();

	void beginRendering(VkRenderingInfoKHR& info);
	void bindPipeline(VkPipelineBindPoint bind_point, VkPipeline pipeline);

	void setViewport(std::vector<VkViewport>& viewport_arr);
	void setViewport(VkViewport* p_viewport, uint32_t count);
	void setScissor(std::vector<VkRect2D>& scissor_arr);
	void setScissor(VkRect2D* p_scissor, uint32_t count);
	void bindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout pipeline_layout, uint32_t first_set, uint32_t descriptor_count, const VkDescriptorSet* p_descriptor_sets,
		uint32_t dynamic_offset_count, const uint32_t* p_dynamic_offsets);
	void pushConstants(const VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, const void* p_values);
	void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type);
	void bindVertexBuffer(uint32_t first_binding, uint32_t binding_count, const VkBuffer* p_buffers, const VkDeviceSize* p_offsets);
	void drawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
	void dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

	void endRendering();

	void beginRendering(
		VkClearColorValue& clear_color,
		Swapchain& swapchain,
		DepthBuffer& depth_buffer,
		VkOffset2D& offset,
		VkExtent2D& extent
	);

	void dynamicRenderingPipelineBarrier(
		Swapchain& swapchain,
		DepthBuffer& depth_buffer
	);
	void dynamicRenderingPipelineBarrierBack(Swapchain& swapchain);

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

	void copyImageToImage(
		VkImage src_image,
		VkImageLayout src_image_layout,
		VkImage dst_image,
		VkImageLayout dst_image_layout,
		uint32_t region_count,
		const VkImageCopy* p_regions
	);
};

