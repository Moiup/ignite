#include "CommandBuffer.h"

CommandBuffer::CommandBuffer() :
	_command_buffer{},
	_level{VK_COMMAND_BUFFER_LEVEL_PRIMARY}
{
	;
}

CommandBuffer::CommandBuffer(
	VkCommandBuffer command_buffer,
	VkCommandBufferLevel level
) :
	_command_buffer{command_buffer},
	_level{level}
{
	;
}

CommandBuffer::CommandBuffer(const CommandBuffer& cmd_buf) :
	_command_buffer {cmd_buf._command_buffer},
	_level{cmd_buf._level}
{
	;
}

void CommandBuffer::setLevel(VkCommandBufferLevel level) {
	_level = level;
}

VkCommandBuffer CommandBuffer::getCommandBuffer() {
	return _command_buffer;
}


VkCommandBufferLevel CommandBuffer::getLevel() {
	return _level;
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
void CommandBuffer::bindPipeline(VkPipelineBindPoint bind_point, VkPipeline pipeline) {
	vkCmdBindPipeline(
		_command_buffer,
		bind_point,
		pipeline
	);
}

void CommandBuffer::bindPipeline(VkPipelineBindPoint bind_point, Pipeline& pp) {
	bindPipeline(bind_point, pp.getPipeline());

	bindDescriptorSets(
		bind_point,
		pp.getPipelineLayout(),
		0,
		pp.getDescriptorSets().size(),
		pp.getDescriptorSets().data(),
		0,
		nullptr
	);

	if (pp.getShader().getPushConstantRange().size) {
		pushConstants(
			pp.getPipelineLayout(),
			pp.getShader().getPushConstantRange().stageFlags,
			pp.getShader().getPushConstantRange().offset,
			pp.getShader().getPushConstantRange().size,
			pp.getPushConstants()
		);
	}
}

void CommandBuffer::bindPipeline(GraphicsPipeline& gp) {
	bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, gp);

	setViewport(
		&gp.getViewport(),
		1
	);
	setScissor(
		&gp.getScissors(),
		1
	);
	setCullMode(gp.getCullMode());
	setFrontFace(gp.getFrontFace());

	bindIndexBuffer(
		gp.getIndexBuffer(),
		0,
		gp.getShader().getIndexBufferInfo().getIndexType()
	);

	VkDeviceSize v_offsets = { 0 };
	for (const auto& vb : gp.getVertexBuffers()) {
		const std::string& buf_name = vb.first;
		const VkBuffer vertex_buffer = vb.second;

		bindVertexBuffer(
			gp.getShader().getVertexBufferDescription(buf_name).binding_desc.binding,
			1,
			&vertex_buffer,
			&v_offsets
		);
	}
}

void CommandBuffer::bindPipeline(ComputePipeline& cp) {
	bindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, cp);
}

void CommandBuffer::setViewport(std::vector<VkViewport>& viewport_arr) {
	vkCmdSetViewport(
		_command_buffer,
		0,
		viewport_arr.size(),
		viewport_arr.data()
	);
}

void CommandBuffer::setViewport(VkViewport* p_viewport, uint32_t count) {
	vkCmdSetViewport(
		_command_buffer,
		0,
		count,
		p_viewport
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

void CommandBuffer::setScissor(VkRect2D* p_scissor, uint32_t count) {
	vkCmdSetScissor(
		_command_buffer,
		0,
		count,
		p_scissor
	);
}

void CommandBuffer::setCullMode(VkCullModeFlags cull_mode){
	vkCmdSetCullMode(
		_command_buffer,
		cull_mode
	);
}

void CommandBuffer::setFrontFace(VkFrontFace front_face){
	vkCmdSetFrontFace(
		_command_buffer,
		front_face
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

void CommandBuffer::pushConstants(const VkPipelineLayout layout, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size, const void* p_values) {
	vkCmdPushConstants(
		_command_buffer,
		layout,
		stage_flags,
		offset,
		size,
		p_values
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

void CommandBuffer::drawIndexed(
	const uint32_t index_count,
	const uint32_t instance_count,
	const uint32_t first_index,
	const uint32_t vertex_offset,
	const uint32_t first_instance
) {
	vkCmdDrawIndexed(
		_command_buffer,
		index_count,
		instance_count,
		first_index,
		vertex_offset,
		first_instance
	);
}

void CommandBuffer::dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z) {
	vkCmdDispatch(
		_command_buffer,
		group_count_x,
		group_count_y,
		group_count_z
	);
}

void CommandBuffer::dispatch(
	ComputePipeline& cp,
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z
) {
	bindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, cp);

	dispatch(
		group_count_x,
		group_count_y,
		group_count_z
	);
}

void CommandBuffer::endRendering() {
	vkCmdEndRendering(_command_buffer);
}

void CommandBuffer::endRendering(Image &image) {
	VkImageLayout image_layout = image.getImageLayout();
	endRendering();
	dynamicRenderingPipelineBarrierBack(image);
	changeLayout(image, image_layout);
}

void CommandBuffer::beginRendering(
	VkClearColorValue clear_color,
	Image& image,
	DepthBuffer& depth_buffer,
	VkOffset2D offset,
	VkExtent2D extent
) {
	dynamicRenderingPipelineBarrier(
		image,
		depth_buffer
	);

	VkRenderingAttachmentInfoKHR color_attachment{};
	color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	color_attachment.imageView = image.getImageView();
	color_attachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.clearValue.color = clear_color;

	VkRenderingAttachmentInfoKHR depth_stencil_attachment{};
	depth_stencil_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	depth_stencil_attachment.imageView = depth_buffer.getImageView();
	depth_stencil_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_stencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_stencil_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_stencil_attachment.clearValue.color.float32[0] = 0.0f;
	depth_stencil_attachment.clearValue.color.float32[1] = 0.0f;
	depth_stencil_attachment.clearValue.color.float32[2] = 0.0f;
	depth_stencil_attachment.clearValue.color.float32[3] = 0.0f;
	depth_stencil_attachment.clearValue.depthStencil.depth = 1.0f;
	depth_stencil_attachment.clearValue.depthStencil.stencil = 0;

	VkRenderingInfoKHR rendering_info_khr{};
	rendering_info_khr.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	rendering_info_khr.pNext = nullptr;
	rendering_info_khr.flags = 0;
	//rendering_info_khr.renderArea = graphics_pipeline.getScissors()[0];
	rendering_info_khr.renderArea.offset = offset;
	rendering_info_khr.renderArea.extent = extent;
	rendering_info_khr.layerCount = 1;
	rendering_info_khr.viewMask = 0;
	rendering_info_khr.colorAttachmentCount = 1;
	rendering_info_khr.pColorAttachments = &color_attachment;
	rendering_info_khr.pDepthAttachment = &depth_stencil_attachment;
	rendering_info_khr.pStencilAttachment = &depth_stencil_attachment;

	beginRendering(
		rendering_info_khr
	);
}

void CommandBuffer::beginRendering(
	VkClearColorValue clear_color,
	Image& image,
	DepthBuffer& depth_buffer,
	VkOffset2D offset
) {
	VkExtent2D extent = {
		image.getWidth(),
		image.getHeight()
	};

	beginRendering(
		clear_color,
		image,
		depth_buffer,
		offset,
		extent
	);
}

void CommandBuffer::dynamicRenderingPipelineBarrier(
	Image& image,
	DepthBuffer& depth_buffer
) {
	VkImageSubresourceRange subresource_range_frame{};
	subresource_range_frame.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range_frame.baseMipLevel = 0;
	subresource_range_frame.levelCount = 1;
	subresource_range_frame.baseArrayLayer = 0;
	subresource_range_frame.layerCount = 1;

	VkImageSubresourceRange depth_subresource_range_frame{};
	depth_subresource_range_frame.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depth_subresource_range_frame.baseMipLevel = 0;
	depth_subresource_range_frame.levelCount = 1;
	depth_subresource_range_frame.baseArrayLayer = 0;
	depth_subresource_range_frame.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier_frame{};
	image_memory_barrier_frame.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier_frame.pNext = nullptr;
	image_memory_barrier_frame.srcAccessMask = 0;
	image_memory_barrier_frame.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	image_memory_barrier_frame.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_memory_barrier_frame.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	image_memory_barrier_frame.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame.image = image.getImage();
	image_memory_barrier_frame.subresourceRange = subresource_range_frame;

	VkImageMemoryBarrier depth_memory_barrier_frame{};
	depth_memory_barrier_frame.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	depth_memory_barrier_frame.pNext = nullptr;
	depth_memory_barrier_frame.srcAccessMask = 0;
	depth_memory_barrier_frame.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	depth_memory_barrier_frame.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_memory_barrier_frame.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depth_memory_barrier_frame.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depth_memory_barrier_frame.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	depth_memory_barrier_frame.image = depth_buffer.getImage();
	depth_memory_barrier_frame.subresourceRange = depth_subresource_range_frame;

	pipelineBarrier(
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier_frame
	);

	pipelineBarrier(
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &depth_memory_barrier_frame
	);
}

void CommandBuffer::dynamicRenderingPipelineBarrierBack(Image& image) {
	VkImageSubresourceRange subresource_range_frame_bk{};
	subresource_range_frame_bk.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range_frame_bk.baseMipLevel = 0;
	subresource_range_frame_bk.levelCount = 1;
	subresource_range_frame_bk.baseArrayLayer = 0;
	subresource_range_frame_bk.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier_frame_bk{};
	image_memory_barrier_frame_bk.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier_frame_bk.pNext = nullptr;
	image_memory_barrier_frame_bk.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	image_memory_barrier_frame_bk.dstAccessMask = 0;
	image_memory_barrier_frame_bk.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	image_memory_barrier_frame_bk.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	image_memory_barrier_frame_bk.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame_bk.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	image_memory_barrier_frame_bk.image = image.getImage();
	image_memory_barrier_frame_bk.subresourceRange = subresource_range_frame_bk;

	pipelineBarrier(
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier_frame_bk
	);

	image.setImageInitialLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

void CommandBuffer::pipelineBarrier(const VkDependencyInfo* p_dependency_info){
	vkCmdPipelineBarrier2(_command_buffer, p_dependency_info);
}

void CommandBuffer::barrier(
	VkPipelineStageFlags2 src_stage_mask,
	VkPipelineStageFlags2 dst_stage_mask,
	VkAccessFlags2 src_access_mask,
	VkAccessFlags2 dst_access_mask
){
	VkMemoryBarrier2 mem_bar{};
	mem_bar.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
	mem_bar.pNext = nullptr;
	mem_bar.srcStageMask = src_stage_mask;
	mem_bar.dstStageMask = dst_stage_mask;
	mem_bar.srcAccessMask = src_access_mask;
	mem_bar.dstAccessMask = dst_access_mask;

	VkDependencyInfo dependency_info{};
	dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency_info.pNext = nullptr;
	dependency_info.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	dependency_info.memoryBarrierCount = 1;
	dependency_info.pMemoryBarriers = &mem_bar;
	dependency_info.bufferMemoryBarrierCount = 0;
	dependency_info.pBufferMemoryBarriers = nullptr;
	dependency_info.imageMemoryBarrierCount = 0;
	dependency_info.pImageMemoryBarriers = nullptr;

	pipelineBarrier(&dependency_info);
}

void CommandBuffer::copyBufferToBuffer(
	VkBuffer srcBuffer,
	VkBuffer dstBuffer,
	uint32_t regionCount,
	const VkBufferCopy2* pRegions
) {
	VkCopyBufferInfo2 info{};
	info.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2;
	info.pNext = nullptr;
	info.srcBuffer = srcBuffer;
	info.dstBuffer = dstBuffer;
	info.regionCount = regionCount;
	info.pRegions = pRegions;

	vkCmdCopyBuffer2(
		_command_buffer,
		&info
	);
}

void CommandBuffer::copyBufferToImage(
	VkBuffer srcBuffer,
	VkImage dstImage,
	VkImageLayout dstImageLayout,
	uint32_t regionCount,
	const VkBufferImageCopy* pRegions
) {
	vkCmdCopyBufferToImage(
		_command_buffer,
		srcBuffer,
		dstImage,
		dstImageLayout,
		regionCount,
		pRegions
	);
}

void CommandBuffer::copyImageToBuffer(
	VkImage src_image,
	VkImageLayout src_image_layout,
	VkBuffer dst_buffer,
	uint32_t region_count,
	const VkBufferImageCopy* p_regions
) {
	vkCmdCopyImageToBuffer(
		_command_buffer,
		src_image,
		src_image_layout,
		dst_buffer,
		region_count,
		p_regions
	);
}

void CommandBuffer::copyImageToBuffer(
	VkCopyImageToBufferInfo2* p_copy_image_to_buffer_info
) {
	vkCmdCopyImageToBuffer2(
		_command_buffer,
		p_copy_image_to_buffer_info
	);
}

void CommandBuffer::copyImageToImage(
	VkImage src_image,
	VkImageLayout src_image_layout,
	VkImage dst_image,
	VkImageLayout dst_image_layout,
	uint32_t region_count,
	const VkImageCopy* p_regions
) {
	vkCmdCopyImage(
		_command_buffer,
		src_image,
		src_image_layout,
		dst_image,
		dst_image_layout,
		region_count,
		p_regions
	);
}

void CommandBuffer::copy(Image& src, Image& dst,
	VkExtent3D extent,
	VkOffset3D src_offset,
	VkOffset3D dst_offset,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	VkImageLayout image_layout = dst.getImageLayout();

	VkImageLayout src_image_layout_initial = src.getImageLayout();
	changeLayout(
		src,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		src_access_mask,
		VK_ACCESS_TRANSFER_READ_BIT,
		src_stage_mask,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

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
	VkImageCopy image_copy{};
	image_copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.srcSubresource.mipLevel = 0;
	image_copy.srcSubresource.baseArrayLayer = 0;
	image_copy.srcSubresource.layerCount = 1;
	image_copy.srcOffset = src_offset;
	image_copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.dstSubresource.mipLevel = 0;
	image_copy.dstSubresource.baseArrayLayer = 0;
	image_copy.dstSubresource.layerCount = 1;
	image_copy.dstOffset = dst_offset;
	image_copy.extent = extent;

	copyImageToImage(
		src.getImage(),
		src.getImageLayout(),
		dst.getImage(),
		dst.getImageLayout(),
		1,
		&image_copy
	);

	changeLayout(
		src,
		src_image_layout_initial,
		src.getStageAccessInfo().access_mask,
		dst_access_mask,
		src.getStageAccessInfo().stage_mask,
		dst_stage_mask
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

void CommandBuffer::copy(Image& src, Image& dst,
	VkOffset3D src_offset,
	VkOffset3D dst_offset,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
){
	VkExtent3D extent = {
		std::min(src.getWidth(), dst.getWidth()),
		std::min(src.getHeight(), dst.getHeight()),
		1
	};
	copy(
		src,
		dst,
		extent,
		src_offset,
		dst_offset,
		src_access_mask,
		dst_access_mask,
		src_stage_mask,
		dst_stage_mask
	);
}

void CommandBuffer::changeLayout(
	Image& img,
	VkImageLayout new_layout,
	VkAccessFlags src_access_mask,
	VkAccessFlags dst_access_mask,
	VkPipelineStageFlags src_stage_mask,
	VkPipelineStageFlags dst_stage_mask
) {
	// Preparing the transfer with the image memory barrier
	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask = img.aspectMask();
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier{};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.image = img.getImage();
	image_memory_barrier.subresourceRange = subresource_range;
	image_memory_barrier.srcAccessMask = src_access_mask;
	image_memory_barrier.dstAccessMask = dst_access_mask;
	image_memory_barrier.oldLayout = img.getImageLayout();
	image_memory_barrier.newLayout = new_layout;

	pipelineBarrier(
		src_stage_mask,
		dst_stage_mask,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier
	);

	img.getStageAccessInfo().access_mask = dst_access_mask;
	img.getStageAccessInfo().stage_mask = dst_stage_mask;

	img.setImageInitialLayout(new_layout);
}
