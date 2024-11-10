#include "Pipeline.h"

Pipeline::Pipeline() :
	_shader{ nullptr },
	_descriptor_set_layout{},
	_descriptor_pool{},
	_descriptor_sets{},
	_pipeline_layout{},
	_pipeline{nullptr}
{
	;
}

Pipeline::Pipeline(Shader& shader) :
	Pipeline::Pipeline()
{
	_shader = &shader;
	create();
}

Pipeline::~Pipeline() {
	destroy();
}

const VkPipeline& Pipeline::getPipeline() const {
	return _pipeline;
}

const VkPipelineLayout& Pipeline::getPipelineLayout() const {
	return _pipeline_layout;
}

const std::vector<VkDescriptorSet>& Pipeline::getDescriptorSets() const {
	return _descriptor_sets;
}

Shader* Pipeline::getShader() {
	return _shader;
}

void Pipeline::create() {
	createDescriptorSet();
	createPipelineLayout();
	createPipeline();
}

void Pipeline::destroy() {
	if (!_pipeline) {
		return;
	}
	destroyPipeline();
	destroyDescriptorSet();
	destroyPipelineLayout();
}

VkWriteDescriptorSet Pipeline::setWriteDescriptorSet(
	const std::string& name
) {
	VkDescriptorSetLayoutBinding infos = _shader->getBuffersInfo().at(name);
	VkWriteDescriptorSet write;
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = nullptr;
	write.dstSet = _descriptor_sets[0];
	write.dstBinding = infos.binding;
	write.dstArrayElement = 0;
	write.descriptorCount = infos.descriptorCount;
	write.descriptorType = infos.descriptorType;
}

void Pipeline::setUniformBuffer(
	const std::string& name,
	const Buffer<IGEBufferUsage::uniform_buffer>& buff
) {
	VkWriteDescriptorSet write = setWriteDescriptorSet(name);
	_descriptor_buffer_infos.push_back(VkDescriptorBufferInfo{});
	VkDescriptorBufferInfo& desc_buf_info = _descriptor_buffer_infos.back();
	desc_buf_info.buffer = buff.getBuffer();
	desc_buf_info.offset = 0;
	desc_buf_info.range = 0;

	write.pBufferInfo = &desc_buf_info;
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;
}

void Pipeline::setStorageBuffer(
	const std::string& name,
	const Buffer<IGEBufferUsage::storage_buffer>& buff
) {
	VkWriteDescriptorSet write = setWriteDescriptorSet(name);
	_descriptor_buffer_infos.push_back(VkDescriptorBufferInfo{});
	VkDescriptorBufferInfo& desc_buf_info = _descriptor_buffer_infos.back();
	desc_buf_info.buffer = buff.getBuffer();
	desc_buf_info.offset = 0;
	desc_buf_info.range = 0;

	write.pBufferInfo = &desc_buf_info;
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;
}

void Pipeline::setSamplers(
	const std::string& name,
	const std::vector<Sampler&>& samp
) {
	VkWriteDescriptorSet write = setWriteDescriptorSet(name);
	uint32_t offset = _descriptor_image_infos.size();
	for (const Sampler& s : samp) {
		_descriptor_image_infos.push_back(VkDescriptorImageInfo{});
		VkDescriptorImageInfo& desc_img_info = _descriptor_image_infos.back();
		desc_img_info.sampler = s.getSampler();
		desc_img_info.imageView = nullptr;
		desc_img_info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	}

	write.pBufferInfo = nullptr;
	write.pImageInfo = _descriptor_image_infos.data() + offset;
	write.pTexelBufferView = nullptr;
}

void Pipeline::setTextures2D(
	const std::string& name,
	const std::vector<Texture2D&>& textures
) {
	VkWriteDescriptorSet write = setWriteDescriptorSet(name);
	uint32_t offset = _descriptor_image_infos.size();
	for (const Texture2D& t : textures) {
		_descriptor_image_infos.push_back(VkDescriptorImageInfo{});
		VkDescriptorImageInfo& desc_img_info = _descriptor_image_infos.back();
		desc_img_info.sampler = t.getSampler();
		desc_img_info.imageView = t.getImageView();
		desc_img_info.imageLayout = t.getImageLayout();
	}

	write.pBufferInfo = nullptr;
	write.pImageInfo = _descriptor_image_infos.data() + offset;
	write.pTexelBufferView = nullptr;
}

void Pipeline::setDescriptorSetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr, const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& buffer_arr) {
	for (auto& name_buff : buffer_arr) {
		const VkDescriptorSetLayoutBinding& buff_info = name_buff.second;
		descriptor_set_binding_arr.push_back(buff_info);
	}
}

void Pipeline::createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr) {
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
	descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_info.pNext = nullptr;
	descriptor_set_layout_info.flags = 0;
	descriptor_set_layout_info.bindingCount = descriptor_set_layout_binding_arr.size();
	descriptor_set_layout_info.pBindings = descriptor_set_layout_binding_arr.data();

	VkDescriptorSetLayout descriptor_set_layout{};
	_descriptor_set_layout.push_back(descriptor_set_layout);
	VkResult vk_result = vkCreateDescriptorSetLayout(
		_shader->getDevice()->getDevice(),
		&descriptor_set_layout_info,
		nullptr,
		_descriptor_set_layout.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating descriptor set layout:!");
	}
}

void Pipeline::createDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr) {
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
		_shader->getDevice()->getDevice(),
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
		_shader->getDevice()->getDevice(),
		&descriptor_sets_info,
		_descriptor_sets.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating descriptor sets!");
	}
}

void Pipeline::update() {
	if (!is_changed) {
		return;
	}

	vkUpdateDescriptorSets(
		_shader->getDevice()->getDevice(),
		_write_descriptor_sets.size(),
		_write_descriptor_sets.data(),
		0,
		nullptr
	);
}

void Pipeline::createDescriptorSet() {
	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_binding_arr;
	setDescriptorSetLayoutBinding(
		descriptor_set_layout_binding_arr,
		_shader->getBuffersInfo()
	);

	if (!descriptor_set_layout_binding_arr.size()) {
		return;
	}
	createDescriptorSetLayout(descriptor_set_layout_binding_arr);
	createDescriptorSet(descriptor_set_layout_binding_arr);
	//updateDescriptorSets();
}

void Pipeline::destroyDescriptorSet() {
	if (_descriptor_pool == VK_NULL_HANDLE) {
		return;
	}

	vkFreeDescriptorSets(
		_shader->getDevice()->getDevice(),
		_descriptor_pool,
		_descriptor_sets.size(),
		_descriptor_sets.data()
	);

	vkDestroyDescriptorPool(
		_shader->getDevice()->getDevice(),
		_descriptor_pool,
		nullptr
	);
	destroyDescriptorSetLayout();
}

void Pipeline::destroyDescriptorSetLayout() {
	for (auto _dsl : _descriptor_set_layout) {
		vkDestroyDescriptorSetLayout(
			_shader->getDevice()->getDevice(),
			_dsl,
			nullptr
		);
	}
}

void Pipeline::createPipelineLayout() {
	std::vector<VkPushConstantRange> push_constant_ranges;

	for (const std::pair<std::string, PushConstantInfo>& str_info : _shader->getPushConstantInfo()) {
		const PushConstantInfo& info = str_info.second;
		const VkPushConstantRange& range = info.getPushConstantRange();
		push_constant_ranges.push_back(range);
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pNext = nullptr;
	pipeline_layout_info.flags = 0;
	pipeline_layout_info.setLayoutCount = _descriptor_set_layout.size();
	pipeline_layout_info.pSetLayouts = _descriptor_set_layout.data();
	pipeline_layout_info.pushConstantRangeCount = push_constant_ranges.size();
	pipeline_layout_info.pPushConstantRanges = push_constant_ranges.data();

	VkResult vk_result = vkCreatePipelineLayout(
		_shader->getDevice()->getDevice(),
		&pipeline_layout_info,
		nullptr,
		&_pipeline_layout
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the pipeline layout!");
	}
}

void Pipeline::destroyPipelineLayout() {
	vkDestroyPipelineLayout(
		_shader->getDevice()->getDevice(),
		_pipeline_layout,
		nullptr
	);
}

void Pipeline::destroyPipeline() {
	vkDestroyPipeline(
		_shader->getDevice()->getDevice(),
		_pipeline,
		nullptr
	);
}