#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline() :
	_logical_device{ nullptr },
	_shader{ nullptr },
	_polygon_mode{ VK_POLYGON_MODE_FILL },
	_cull_mode{ VK_CULL_MODE_NONE },
	_front_face{ VK_FRONT_FACE_CLOCKWISE },
	_line_width{ 1.0f },
	_descriptor_set_layout{},
	_pipeline_layout{ nullptr },
	_descriptor_pool{ nullptr },
	_descriptor_sets{},
	_viewport_arr{},
	_scissor_arr{}
{ ; }

void GraphicsPipeline::setLogicalDevice(VkDevice* logical_device) {
	_logical_device = logical_device;
}

void GraphicsPipeline::setPhysicalDevice(PhysicalDevice* physical_device) {
	_physical_device = physical_device;
}

void GraphicsPipeline::setShader(GraphicShader* shader) {
	_shader = shader;
}

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

const VkPipeline& GraphicsPipeline::getPipeline() const {
	return _pipeline;
}

const VkPipelineLayout& GraphicsPipeline::getPipelineLayout() const {
	return _pipeline_layout;
}

const std::vector<VkDescriptorSet>& GraphicsPipeline::getDescriptorSets() const {
	return _descriptor_sets;
}

const std::vector<VkViewport>& GraphicsPipeline::getViewport() const {
	return _viewport_arr;
}

const std::vector<VkRect2D>& GraphicsPipeline::getScissors() const {
	return _scissor_arr;
}

GraphicShader* GraphicsPipeline::getShader() {
	return _shader;
}

void GraphicsPipeline::create() {
	createDescriptorSet();
	createPipelineLayout();
	createPipeline();
}

void GraphicsPipeline::destroy() {
	destroyPipeline();
	destroyDescriptorSet();
	destroyPipelineLayout();
}

void GraphicsPipeline::setDescriptorSetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr, std::unordered_map<std::string, ArrayBufferInfo>& buffer_arr) {
	for (auto& name_buff : buffer_arr) {
		ArrayBufferInfo& buff_info = name_buff.second;
		VkDescriptorSetLayoutBinding descriptor_set_binding{};
		descriptor_set_binding.binding = buff_info.getBinding();
		descriptor_set_binding.descriptorType = buff_info.getDescriptorType();
		descriptor_set_binding.descriptorCount = buffer_arr.count(name_buff.first);
		descriptor_set_binding.stageFlags = buff_info.getStageFlags();
		descriptor_set_binding.pImmutableSamplers = nullptr;
		descriptor_set_binding_arr.push_back(descriptor_set_binding);
	}
}

std::vector<VkDescriptorSetLayoutBinding> GraphicsPipeline::setDescriptorSetLayoutBindingArray() {
	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_binding_arr;
	setDescriptorSetLayoutBinding(descriptor_set_layout_binding_arr, _shader->getUniformBuffersInfo());
	setDescriptorSetLayoutBinding(descriptor_set_layout_binding_arr, _shader->getStorageBuffersInfo());

	return descriptor_set_layout_binding_arr;
}

void GraphicsPipeline::createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr) {
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
	descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_info.pNext = nullptr;
	descriptor_set_layout_info.flags = 0;
	descriptor_set_layout_info.bindingCount = descriptor_set_layout_binding_arr.size();
	descriptor_set_layout_info.pBindings = descriptor_set_layout_binding_arr.data();
	
	VkDescriptorSetLayout descriptor_set_layout{};
	_descriptor_set_layout.push_back(descriptor_set_layout);
	VkResult vk_result = vkCreateDescriptorSetLayout(
		*_logical_device,
		&descriptor_set_layout_info,
		nullptr,
		_descriptor_set_layout.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating descriptor set layout:!");
	}
}

void GraphicsPipeline::createDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr) {
	std::vector<VkDescriptorPoolSize> descriptor_pool_size_arr;
	
	for (VkDescriptorSetLayoutBinding& dslb : descriptor_set_layout_binding_arr) {
		VkDescriptorPoolSize descriptor_pool_size{};
		descriptor_pool_size.type = dslb.descriptorType;
		descriptor_pool_size.descriptorCount = dslb.descriptorCount;
		descriptor_pool_size_arr.push_back(descriptor_pool_size);
	}

	VkDescriptorPoolCreateInfo descriptor_pool_info{};
	descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_info.pNext = nullptr;
	descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptor_pool_info.maxSets = descriptor_pool_size_arr.size();
	descriptor_pool_info.poolSizeCount = descriptor_pool_size_arr.size();
	descriptor_pool_info.pPoolSizes = descriptor_pool_size_arr.data();

	VkResult vk_result = vkCreateDescriptorPool(
		*_logical_device,
		&descriptor_pool_info,
		nullptr,
		&_descriptor_pool
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the descriptor pool!");
	}

	_descriptor_sets.resize(_descriptor_set_layout.size());

	VkDescriptorSetAllocateInfo descriptor_sets_info{};
	descriptor_sets_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_sets_info.pNext = nullptr;
	descriptor_sets_info.descriptorPool = _descriptor_pool;
	descriptor_sets_info.descriptorSetCount = _descriptor_set_layout.size();
	descriptor_sets_info.pSetLayouts = _descriptor_set_layout.data();

	vk_result = vkAllocateDescriptorSets(
		*_logical_device,
		&descriptor_sets_info,
		_descriptor_sets.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating descriptor sets!");
	}
}

void GraphicsPipeline::setWriteDescriptorSet(std::unordered_map<std::string, ArrayBuffer*>& buffer_arr, std::unordered_map<std::string, ArrayBufferInfo>& buffer_info_arr, std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr) {
	for (std::pair<std::string, ArrayBuffer*> buff_data : buffer_arr) {
		std::string name = buff_data.first;
		ArrayBuffer* buff = buff_data.second;
		ArrayBufferInfo& info = buffer_info_arr[name];
		VkDescriptorBufferInfo* descriptor_buffer_info = new VkDescriptorBufferInfo();

		descriptor_buffer_info->buffer = buff->getBuffer();
		descriptor_buffer_info->offset = 0;
		descriptor_buffer_info->range = VK_WHOLE_SIZE;
		
		VkWriteDescriptorSet writes{};
		writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes.pNext = nullptr;
		writes.dstSet = _descriptor_sets[0];
		writes.dstBinding = info.getBinding();
		writes.dstArrayElement = 0;
		writes.descriptorCount = 1;
		writes.descriptorType = info.getDescriptorType();
		writes.pImageInfo = nullptr;
		writes.pBufferInfo = descriptor_buffer_info;
		writes.pTexelBufferView = nullptr;
		
		write_descriptor_set_arr.push_back(writes);
	}	
}

void GraphicsPipeline::updateDescriptorSets() {
	std::vector<VkWriteDescriptorSet> write_descriptor_set_arr;
	setWriteDescriptorSet(
		(std::unordered_map<std::string, ArrayBuffer*>&)_shader->getUniformBuffers(),
		_shader->getUniformBuffersInfo(),
		write_descriptor_set_arr
	);

	setWriteDescriptorSet(
		(std::unordered_map<std::string, ArrayBuffer*>&)_shader->getStorageBuffers(),
		_shader->getStorageBuffersInfo(),
		write_descriptor_set_arr
	);

	vkUpdateDescriptorSets(
		*_logical_device,
		write_descriptor_set_arr.size(),
		write_descriptor_set_arr.data(),
		0,
		nullptr
	);

	for (VkWriteDescriptorSet& w : write_descriptor_set_arr) {
		delete w.pBufferInfo;
	}
}

void GraphicsPipeline::createDescriptorSet() {
	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_binding_arr = setDescriptorSetLayoutBindingArray();
	if (!descriptor_set_layout_binding_arr.size()) {
		return;
	}
	createDescriptorSetLayout(descriptor_set_layout_binding_arr);
	createDescriptorSet(descriptor_set_layout_binding_arr);
	updateDescriptorSets();
}

void GraphicsPipeline::destroyDescriptorSet() {
	if (_descriptor_pool == VK_NULL_HANDLE) {
		return;
	}

	vkFreeDescriptorSets(
		*_logical_device,
		_descriptor_pool,
		_descriptor_sets.size(),
		_descriptor_sets.data()
	);

	vkDestroyDescriptorPool(
		*_logical_device,
		_descriptor_pool,
		nullptr
	);
	destroyDescriptorSetLayout();
}

void GraphicsPipeline::destroyDescriptorSetLayout() {
	for (auto _dsl : _descriptor_set_layout) {
		vkDestroyDescriptorSetLayout(
			*_logical_device,
			_dsl,
			nullptr
		);
	}
}

void GraphicsPipeline::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pNext = nullptr;
	pipeline_layout_info.flags = 0;
	pipeline_layout_info.setLayoutCount = _descriptor_set_layout.size();
	pipeline_layout_info.pSetLayouts = _descriptor_set_layout.data();
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;
	
	VkResult vk_result = vkCreatePipelineLayout(
		*_logical_device,
		&pipeline_layout_info,
		nullptr,
		&_pipeline_layout
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the pipeline layout!");
	}
}

void GraphicsPipeline::createPipeline() {
	//---- Vertex Input State (22.2)----//
	std::vector<VkVertexInputBindingDescription> vertex_input_binding_desc_arr = {};
	std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_arr{};

	uint32_t binding_i = 0;
	for (auto& vert_buff : _shader->getVertexBuffersInfo()) {
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
	depth_stencil_state_info.back = depth_stencil_state_info.front;
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
	pipeline_info.stageCount = _shader->getShaderStages().size();
	pipeline_info.pStages = _shader->getShaderStages().data();
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
		*_logical_device,
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

void GraphicsPipeline::destroyPipelineLayout() {
	vkDestroyPipelineLayout(
		*_logical_device,
		_pipeline_layout,
		nullptr
	);
}

void GraphicsPipeline::destroyPipeline() {
	vkDestroyPipeline(
		*_logical_device,
		_pipeline,
		nullptr
	);
}