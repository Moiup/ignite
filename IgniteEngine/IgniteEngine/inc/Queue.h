#pragma once

#include "PhysicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "Image.h"
#include "ComputePipeline.h"
#include "GraphicsPipeline.h"
#include <vector>
#include <list>
#include <unordered_map>

struct CommandPoolSubmitBuffersIndices {
	uint32_t start_i{0};
	uint32_t nb_cmd_buf{0};
};

class Queue
{
private:
	//static std::unordered_map<VkQueue, std::vector<CommandPool>> _cmd_pools;
	//static std::unordered_map<CommandPool*, std::vector<VkCommandBuffer>> _pending_command_buffers;
	//static std::unordered_map<CommandPool*, CommandPoolSubmitBuffersIndices> _command_pool_indices;

	VkQueue _queue{ nullptr };
	Device* _device{ nullptr };
	VkFence _fence{ nullptr };

	uint32_t _family_index{};
	uint32_t _queue_index{};

	CommandPool _command_pool;
	CommandPoolSubmitBuffersIndices _command_pool_indices;

	std::list<CommandPool>* _command_pools{nullptr};
	std::vector<VkCommandBuffer>* _pending_command_buffers{nullptr};

	int32_t* _shared_count{nullptr};

public:
	Queue();
	Queue(
		Device& device,
		uint32_t family_index,
		uint32_t queue_index
	);
	Queue(const Queue& q);
	Queue(Queue&& q);

	~Queue();

	Queue& operator=(const Queue& q);
	Queue& operator=(Queue&& q);

	void setDevice(Device* device);
	void setFamilyIndex(uint32_t family_index);
	void create();

	const CommandPool& getCommandPool() const;
	CommandPool& getCommandPool();
	VkQueue getQueue();
	Device* getDevice();
	uint32_t getFamilyIndex();
	std::vector<VkCommandBuffer>& getPendingCommandBuffers();
	CommandBuffer& newCommandBuffer();

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
	//void copy(Image& src, Image& dst,
	//	VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
	//	VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
	//	VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	//);
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

	void dispatch(
		ComputePipeline& cp,
		uint32_t group_count_x,	
		uint32_t group_count_y,	
		uint32_t group_count_z	
	);

	void dispatchBarrier(
		ComputePipeline& cp,
		uint32_t group_count_x,
		uint32_t group_count_y,
		uint32_t group_count_z,
		VkPipelineStageFlags2 src_stage_mask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		VkPipelineStageFlags2 dst_stage_mask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		VkAccessFlags2 src_access_mask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
		VkAccessFlags2 dst_access_mask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT
	);

	void barrier(
		VkPipelineStageFlags2 src_stage_mask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		VkPipelineStageFlags2 dst_stage_mask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
		VkAccessFlags2 src_access_mask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT,
		VkAccessFlags2 dst_access_mask = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT
	);

	void beginRendering(
		glm::vec4& clear_color_value,
		Image& image,
		DepthBuffer& depth_buffer,
		VkOffset2D offset = {0, 0}
	);
	void beginRendering(
		glm::vec4& clear_color_value,
		Image& image,
		DepthBuffer& depth_buffer,
		VkOffset2D& offset,
		VkExtent2D& extent
	);
	void bindPipeline(GraphicsPipeline& gp);

	void drawIndexed(
		uint32_t index_count,
		uint32_t instance_count,
		uint32_t first_index,
		uint32_t vertex_offset,
		uint32_t first_instance
	);
	
	void endRendering(Image& swapchain);

	void flush(
		uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo = nullptr
	);

	const void submit(
		uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo = nullptr
	);

	const void submit(
		uint32_t waitSemaphorecount,
		const VkSemaphore* pWaitSemaphores,
		const VkPipelineStageFlags* pWaitDstStageMask,
		uint32_t commandBufferCount,
		const VkCommandBuffer* pCommandBuffers,
		uint32_t signalSemaphoreCount,
		const VkSemaphore* pSignalSemaphores,
		VkFence fence,
		const VkTimelineSemaphoreSubmitInfo* timelineSemaphoreSubmitInfo
	);

	const void wait();

	const void present(
		uint32_t waitSemaphoreCount,
		const VkSemaphore* pWaitSemaphores,
		uint32_t swapchainCount,
		const VkSwapchainKHR* pSwapchains,
		const uint32_t* pImageIndices,
		VkResult* pResults = nullptr
	) const;
	
	void waitIdle();

private:
	void createFence(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
	void addCommandPool();
	void removeCommandPool();
	
	void destroy();

	// Methods that does not create command buffer
	void changeLayout(
		CommandBuffer& cmd_buf,
		Image& img,
		VkImageLayout new_layout,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	void dispatch(
		CommandBuffer& cmd_buf,
		ComputePipeline& cp,
		uint32_t group_count_x,
		uint32_t group_count_y,
		uint32_t group_count_z
	);


	// Command buffers managing methods
	VkCommandBuffer* getPendingCommandBufferStartPointer();
	const uint32_t getStartIndexPendingCommandBuffers() const;
	uint32_t& getStartIndexPendingCommandBuffers();
	const uint32_t getNbPendingCommandBuffers() const;
	uint32_t& getNbPendingCommandBuffers();

};

template<IGEBufferUsage U1, IGEBufferUsage U2>
void Queue::copy(Buffer<U1>& src, Buffer<U2>& dst) {
	
	CommandBuffer cmd_buf = newCommandBuffer();
	cmd_buf.begin();
	
	VkBufferCopy2 copy_info{};
	copy_info.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2;
	copy_info.pNext = nullptr;
	copy_info.srcOffset = 0;
	copy_info.dstOffset = 0;
	copy_info.size = src.getSize();

	cmd_buf.copyBufferToBuffer(
		src.getBuffer(),
		dst.getBuffer(),
		1,
		&copy_info
	);

	cmd_buf.end();
}

template<IGEBufferUsage U>
void Queue::copy(Image& src, Buffer<U>& dst,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	CommandBuffer& cmd_buf = newCommandBuffer();
	VkImageLayout image_layout = src.getImageLayout();

	cmd_buf.begin();

	changeLayout(
		cmd_buf,
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

	cmd_buf.copyImageToBuffer(&copy_info);

	changeLayout(
		cmd_buf,
		src,
		image_layout,
		src.getStageAccessInfo().access_mask,
		dst_access_mask,
		src.getStageAccessInfo().stage_mask,
		dst_stage_mask
	);

	cmd_buf.end();
}

template<IGEBufferUsage U>
void Queue::copy(Buffer<U>& src, Image& dst,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	CommandBuffer& cmd_buf = newCommandBuffer();

	VkImageLayout image_layout = dst.getImageLayout();

	cmd_buf.begin();
	changeLayout(
		cmd_buf,
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

	cmd_buf.copyBufferToImage(
		src.getBuffer(),
		dst.getImage(),
		dst.getImageLayout(),
		1,
		&image_copy
	);

	changeLayout(
		cmd_buf,
		dst,
		image_layout,
		dst.getStageAccessInfo().access_mask,
		dst_access_mask,
		dst.getStageAccessInfo().stage_mask,
		dst_stage_mask
	);

	cmd_buf.end();
}