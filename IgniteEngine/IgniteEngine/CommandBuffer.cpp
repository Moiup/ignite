#include "CommandBuffer.h"

CommandBuffer::CommandBuffer() :
	_command_buffer{},
	_logical_device{nullptr},
	_command_pool{nullptr},
	_level{},
	_created{false}
{
	;
}

void CommandBuffer::setLogicalDevice(VkDevice* logical_device) {
	_logical_device = logical_device;
}

void CommandBuffer::setCommandPool(VkCommandPool* command_pool) {
	_command_pool = command_pool;
}

void CommandBuffer::setLevel(VkCommandBufferLevel level) {
	_level = level;
}

void CommandBuffer::create() {
	VkCommandBufferAllocateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = nullptr;
	info.commandPool = *_command_pool;
	info.commandBufferCount = 1;
	info.level = _level;

	VkResult vk_result = vkAllocateCommandBuffers(
		*_logical_device,
		&info,
		&_command_buffer
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating command buffer!");
	}

	_created = true;
}

void CommandBuffer::free() {
	if (!_created) {
		return;
	}

	vkFreeCommandBuffers(*_logical_device, *_command_pool, 1, &_command_buffer);
}

void CommandBuffer::begin() {
	begin(0);
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pNext = nullptr;
	begin_info.flags = flags;
	begin_info.pInheritanceInfo = nullptr;

	VkResult vk_result = vkBeginCommandBuffer(
		_command_buffer,
		&begin_info
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: faild begining command buffer recording!");
	}
}

void CommandBuffer::end() {
	vkEndCommandBuffer(_command_buffer);
}

void CommandBuffer::reset() {
	vkResetCommandBuffer(
		_command_buffer,
		0
	);
}

void CommandBuffer::beginRendering(VkRenderingInfoKHR& info) {
	vkCmdBeginRendering(
		_command_buffer,
		&info
	);
}
void CommandBuffer::bindPipeline(VkPipelineBindPoint bind_point, VkPipeline& pipeline) {
	vkCmdBindPipeline(
		_command_buffer,
		bind_point,
		pipeline
	);
}

void CommandBuffer::setViewport(std::vector<VkViewport>& viewport_arr) {
	vkCmdSetViewport(
		_command_buffer,
		0,
		viewport_arr.size(),
		viewport_arr.data()
	);
}

void CommandBuffer::setScissor(std::vector<VkRect2D>& scissor_arr) {
	vkCmdSetScissor(
		_command_buffer,
		0,
		scissor_arr.size(),
		scissor_arr.data()
	);
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout pipeline_layout, uint32_t first_set, uint32_t descriptor_count, const VkDescriptorSet* p_descriptor_sets,
	uint32_t dynamic_offset_count, const uint32_t* p_dynamic_offsets) {
	vkCmdBindDescriptorSets(
		_command_buffer,
		bind_point,
		pipeline_layout,
		first_set,
		descriptor_count,
		p_descriptor_sets,
		dynamic_offset_count,
		p_dynamic_offsets
	);
}

void CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type){
	vkCmdBindIndexBuffer(
		_command_buffer,
		buffer,
		offset,
		index_type
	);
}

void CommandBuffer::bindVertexBuffer(uint32_t first_binding, uint32_t binding_count, const VkBuffer* p_buffers, const VkDeviceSize* p_offsets) {
	vkCmdBindVertexBuffers(
		_command_buffer,
		first_binding,
		binding_count,
		p_buffers,
		p_offsets
	);
}

void CommandBuffer::drawIndexed(uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) {
	vkCmdDrawIndexed(
		_command_buffer,
		index_count,
		instance_count,
		first_index,
		vertex_offset,
		first_instance
	);
}

void CommandBuffer::endRendering() {
	vkCmdEndRendering(_command_buffer);
}

void CommandBuffer::pipelineBarrier(
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask,
	VkDependencyFlags dependencyFlags,
	uint32_t memoryBarrierCount,
	const VkMemoryBarrier* pMemoryBarriers,
	uint32_t bufferMemoryBarrierCount,
	const VkBufferMemoryBarrier* pBufferMemoryBarriers,
	uint32_t imageMemoryBarrierCount,
	const VkImageMemoryBarrier* pImageMemoryBarriers
) {
	vkCmdPipelineBarrier(
		_command_buffer,
		srcStageMask,
		dstStageMask,
		dependencyFlags,
		memoryBarrierCount,
		pMemoryBarriers,
		bufferMemoryBarrierCount,
		pBufferMemoryBarriers,
		imageMemoryBarrierCount,
		pImageMemoryBarriers
	);
}

void CommandBuffer::copyBufferToImage(
	VkCommandBuffer commandBuffer,
	VkBuffer srcBuffer,
	VkImage dstImage,
	VkImageLayout dstImageLayout,
	uint32_t regionCount,
	const VkBufferImageCopy* pRegions
) {
	vkCmdCopyBufferToImage(
		commandBuffer,
		srcBuffer,
		dstImage,
		dstImageLayout,
		regionCount,
		pRegions
	);
}

const VkCommandBuffer* CommandBuffer::getCommandBuffer() {
	return &_command_buffer;
}