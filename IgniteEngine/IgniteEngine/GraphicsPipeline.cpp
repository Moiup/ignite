#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline() :
	_polygon_mode{ VK_POLYGON_MODE_FILL },
	_cull_mode{ VK_CULL_MODE_NONE },
	_front_face{ VK_FRONT_FACE_CLOCKWISE },
	_line_width{ 1.0f },
	_viewport_arr{},
	_scissor_arr{}
{ ; }

void GraphicsPipeline::setNbFrame(uint32_t nb_frame) {
	_nb_frame = nb_frame;
}

void GraphicsPipeline::setSwapchain(Swapchain* swapchain) {
	_swapchain = swapchain;
}

void GraphicsPipeline::setDepthBuffer(DepthBuffer* depth_buffer) {
	_depth_buffer = depth_buffer;
}

void GraphicsPipeline::setPolygonMode(VkPolygonMode polygon_mode) {
	_polygon_mode = polygon_mode;
}

void GraphicsPipeline::setCullMode(VkCullModeFlags cull_mode) {
	_cull_mode = cull_mode;
}

void GraphicsPipeline::setFrontFace(VkFrontFace front_face) {
	_front_face = front_face;
}

void GraphicsPipeline::setLineWidth(float line_width) {
	_line_width = line_width;
}

GraphicShader* GraphicsPipeline::getShader()
{
	return (GraphicShader*)_shader;
}

const std::vector<VkViewport>& GraphicsPipeline::getViewport() const {
	return _viewport_arr;
}

const std::vector<VkRect2D>& GraphicsPipeline::getScissors() const {
	return _scissor_arr;
}

void GraphicsPipeline::createPipeline() {
	//---- Vertex Input State (22.2)----//
	std::vector<VkVertexInputBindingDescription> vertex_input_binding_desc_arr = {};
	std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_arr{};

	uint32_t binding_i = 0;
	for (auto& vert_buff : getShader()->getVertexBuffersInfo()) {
		VkVertexInputBindingDescription input_binding_desc{};
		input_binding_desc.binding = binding_i;
		input_binding_desc.stride = vert_buff.second.getStride();
		input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription input_desc{};
		input_desc.location = vert_buff.second.getLocation();
		input_desc.binding = input_binding_desc.binding;
		input_desc.format = vert_buff.second.getFormat();
		input_desc.offset = 0;

		vert_buff.second.setFirstBinding(binding_i);

		vertex_input_binding_desc_arr.push_back(input_binding_desc);
		vertex_input_attribute_arr.push_back(input_desc);
		binding_i++;
	}

	VkPipelineVertexInputStateCreateInfo vertex_input_state_info{};
	vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_info.pNext = nullptr;
	vertex_input_state_info.flags = 0;
	vertex_input_state_info.vertexBindingDescriptionCount = vertex_input_binding_desc_arr.size();
	vertex_input_state_info.pVertexBindingDescriptions = vertex_input_binding_desc_arr.data();
	vertex_input_state_info.vertexAttributeDescriptionCount = vertex_input_attribute_arr.size();
	vertex_input_state_info.pVertexAttributeDescriptions = vertex_input_attribute_arr.data();

	//---- Input Assembly State (22.2.)----//
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info{};
	input_assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state_info.pNext = nullptr;
	input_assembly_state_info.flags = 0;
	input_assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

	//---- Viewport State (26.9.)----//
	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)_swapchain->getSwapchainInfo().imageExtent.width;
	viewport.height = (float)_swapchain->getSwapchainInfo().imageExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0;

	_viewport_arr.push_back(viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = _swapchain->getSwapchainInfo().imageExtent;

	_scissor_arr.push_back(scissor);

	VkPipelineViewportStateCreateInfo viewport_state_info{};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.pNext = nullptr;
	viewport_state_info.flags = 0;
	viewport_state_info.viewportCount = _viewport_arr.size();
	viewport_state_info.pViewports = _viewport_arr.data();
	viewport_state_info.scissorCount = _scissor_arr.size();
	viewport_state_info.pScissors = _scissor_arr.data();

	//---- Rasterization State (27.)----//
	VkPipelineRasterizationStateCreateInfo rasterization_state_info{};
	rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_info.pNext = nullptr;
	rasterization_state_info.flags = 0;
	rasterization_state_info.depthClampEnable = VK_FALSE;
	rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_info.polygonMode = _polygon_mode;
	rasterization_state_info.cullMode = _cull_mode;
	rasterization_state_info.frontFace = _front_face;
	rasterization_state_info.depthBiasEnable = VK_FALSE;
	rasterization_state_info.depthBiasConstantFactor = 0.0;
	rasterization_state_info.depthBiasClamp = 0.0;
	rasterization_state_info.depthBiasSlopeFactor = 0.0;
	rasterization_state_info.lineWidth = 1.0;

	//---- Multisample State ()----//
	VkPipelineMultisampleStateCreateInfo multisample_state_info{};
	multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_info.pNext = nullptr;
	multisample_state_info.flags = 0;
	multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_info.sampleShadingEnable = VK_FALSE;
	multisample_state_info.minSampleShading = 0;
	multisample_state_info.pSampleMask = nullptr;
	multisample_state_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_info.alphaToOneEnable = VK_FALSE;

	//---- Depth Stencil State ()----//
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info{};
	depth_stencil_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state_info.pNext = nullptr;
	depth_stencil_state_info.flags = 0;
	depth_stencil_state_info.depthTestEnable = VK_TRUE;
	depth_stencil_state_info.depthWriteEnable = VK_TRUE;
	depth_stencil_state_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_state_info.stencilTestEnable = VK_FALSE;
	depth_stencil_state_info.front.failOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.passOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.compareOp = VK_COMPARE_OP_ALWAYS;
	depth_stencil_state_info.front.compareMask = 0;
	depth_stencil_state_info.front.reference = 0;
	depth_stencil_state_info.front.depthFailOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.writeMask = 0;
	depth_stencil_state_info.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depth_stencil_state_info.minDepthBounds = 0;
	depth_stencil_state_info.maxDepthBounds = 0;

	//---- Color Blend State ()----//
	std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_arr;
	VkPipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = 0xf;

	color_blend_attachment_arr.push_back(color_blend_attachment);

	VkPipelineColorBlendStateCreateInfo color_blend_state_info{};
	color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_info.pNext = nullptr;
	color_blend_state_info.flags = 0;
	color_blend_state_info.logicOpEnable = VK_FALSE;
	color_blend_state_info.logicOp = VK_LOGIC_OP_CLEAR;
	color_blend_state_info.attachmentCount = color_blend_attachment_arr.size();
	color_blend_state_info.pAttachments = color_blend_attachment_arr.data();
	color_blend_state_info.blendConstants[0] = 1.0;
	color_blend_state_info.blendConstants[1] = 1.0;
	color_blend_state_info.blendConstants[2] = 1.0;
	color_blend_state_info.blendConstants[3] = 1.0;

	//---- Dynamic State (10.11.)----//
	std::vector<VkDynamicState> dynamic_state_arr = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.pNext = nullptr;
	dynamic_state_info.flags = 0;
	dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_state_arr.size());
	dynamic_state_info.pDynamicStates = dynamic_state_arr.data();

	VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info{};
	pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	pipeline_rendering_create_info.colorAttachmentCount = 1;
	pipeline_rendering_create_info.pColorAttachmentFormats = &_swapchain->getSwapchainInfo().imageFormat;
	pipeline_rendering_create_info.depthAttachmentFormat = _depth_buffer->getImageFormat();
	pipeline_rendering_create_info.stencilAttachmentFormat = _depth_buffer->getImageFormat();

	//---- Creating the pipeline (10.2.)----//
	VkGraphicsPipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.pNext = &pipeline_rendering_create_info;
	pipeline_info.flags = 0;
	pipeline_info.stageCount = getShader()->getShaderStages().size();
	pipeline_info.pStages = getShader()->getShaderStages().data();
	pipeline_info.pVertexInputState = &vertex_input_state_info;
	pipeline_info.pInputAssemblyState = &input_assembly_state_info;
	pipeline_info.pTessellationState = nullptr;
	pipeline_info.pViewportState = &viewport_state_info;
	pipeline_info.pRasterizationState = &rasterization_state_info;
	pipeline_info.pMultisampleState = &multisample_state_info;
	pipeline_info.pDepthStencilState = &depth_stencil_state_info;
	pipeline_info.pColorBlendState = &color_blend_state_info;
	pipeline_info.pDynamicState = &dynamic_state_info;
	pipeline_info.layout = _pipeline_layout;
	pipeline_info.renderPass = nullptr;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = 0;

	VkResult vk_result = vkCreateGraphicsPipelines(
		_shader->getLogicalDevice()->getDevice()->getDevice(),
		VK_NULL_HANDLE,
		1,
		&pipeline_info,
		nullptr,
		&_pipeline
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create graphic pipeline!");
	}
}

void GraphicsPipeline::destroyPipeline() {
	vkDestroyPipeline(
		_shader->getLogicalDevice()->getDevice()->getDevice(),
		_pipeline,
		nullptr
	);
}
