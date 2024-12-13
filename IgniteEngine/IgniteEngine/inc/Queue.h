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

	CommandPool _command_pool;
	CommandPoolSubmitBuffersIndices _command_pool_indices;

	std::list<CommandPool>* _command_pools{nullptr};
	std::vector<VkCommandBuffer>* _pending_command_buffers{nullptr};

	int32_t* _shared_count{nullptr};

public:
	Queue();
	Queue(const Queue& q);
	//Queue(Queue&& q);

	~Queue();

	Queue& operator=(const Queue& q);

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

	//void copy(Buffer& src, Buffer& dst,
	//	VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
	//	VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
	//	VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	//);
	template<IGEBufferUsage U>
	void copy(Buffer<U>& src, Image& dst,
		VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);
	//void copy(Image& src, Buffer& dst,
	//	VkAccessFlags src_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
	//	VkAccessFlags dst_access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
	//	VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	//);
	void copy(Image& src, Image& dst,
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

	void beginRendering(
		glm::vec4& clear_color_value,
		Swapchain& swapchain,
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
	void endRendering(Swapchain& swapchain);

	void flush();

	const void submit(
		uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr
	);

	/**
	* Flush but with parameters
	*/
	const void submitNoFence(
		uint32_t waitSemaphorecount = 0,
		const VkSemaphore* pWaitSemaphores = nullptr,
		const VkPipelineStageFlags* pWaitDstStageMask = nullptr,
		uint32_t signalSemaphoreCount = 0,
		const VkSemaphore* pSignalSemaphores = nullptr
	);

	const void submit(
		uint32_t waitSemaphorecount,
		const VkSemaphore* pWaitSemaphores,
		const VkPipelineStageFlags* pWaitDstStageMask,
		uint32_t commandBufferCount,
		const VkCommandBuffer* pCommandBuffers,
		uint32_t signalSemaphoreCount,
		const VkSemaphore* pSignalSemaphores,
		VkFence fence
	) const;

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
	void createFence();
	void addCommandPool(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
	void removeCommandPool();

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
	VkCommandBuffer* Queue::getPendingCommandBufferStartPointer();
	const uint32_t Queue::getStartIndexPendingCommandBuffers() const;
	uint32_t& Queue::getStartIndexPendingCommandBuffers();
	const uint32_t Queue::getNbPendingCommandBuffers() const;
	uint32_t& Queue::getNbPendingCommandBuffers();

};

template<IGEBufferUsage U>
void Queue::copy(Buffer<U>& src, Image& dst,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	CommandBuffer cmd_buf = newCommandBuffer();

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
	image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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