#include "Pipeline.h"

Pipeline::Pipeline() :
	_shader{ nullptr },
	_descriptor_set_layout{},
	_descriptor_pool{},
	_descriptor_sets{},
	_pipeline_layout{},
	_pipeline{}
{
	;
}

Pipeline::~Pipeline() {
	//destroy();
}

void Pipeline::setShader(Shader* shader) {
	_shader = shader;
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

void Pipeline::setDescriptorSetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr, std::unordered_map<std::string, ArrayBufferInfo>& buffer_arr) {
	for (auto& name_buff : buffer_arr) {
		ArrayBufferInfo& buff_info = name_buff.second;
		VkDescriptorSetLayoutBinding descriptor_set_binding{};
		descriptor_set_binding.binding = buff_info.getBinding();
		descriptor_set_binding.descriptorType = buff_info.getDescriptorType();
		descriptor_set_binding.descriptorCount = buff_info.getDescriptorCount();
		descriptor_set_binding.stageFlags = buff_info.getStageFlags();
		descriptor_set_binding.pImmutableSamplers = nullptr;
		descriptor_set_binding_arr.push_back(descriptor_set_binding);
	}
}

std::vector<VkDescriptorSetLayoutBinding> Pipeline::setDescriptorSetLayoutBindingArray() {
	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_binding_arr;

	// Uniform buffers
	setDescriptorSetLayoutBinding(
		descriptor_set_layout_binding_arr,
		_shader->getUniformBuffersInfo()
	);

	// Storage buffers
	setDescriptorSetLayoutBinding(
		descriptor_set_layout_binding_arr,
		_shader->getStorageBuffersInfo()
	);

	// Sampler
	setDescriptorSetLayoutBinding(
		descriptor_set_layout_binding_arr,
		(std::unordered_map<std::string, ArrayBufferInfo>&)_shader->getSamplerInfo()
	);

	// Textures
	setDescriptorSetLayoutBinding(
		descriptor_set_layout_binding_arr,
		(std::unordered_map<std::string, ArrayBufferInfo>&)_shader->getTextureInfo()
	);

	return descriptor_set_layout_binding_arr;
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

void Pipeline::setWriteDescriptorSet(
	std::unordered_map<std::string, std::vector<Sampler*>>& sampler_arr,
	std::unordered_map<std::string, SamplerInfo>& sampler_info_arr,
	std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
) {
	for (auto& sampler_data : sampler_arr) {
		std::string name = sampler_data.first;
		std::vector<Sampler*>& samplers = sampler_data.second;
		SamplerInfo& info = sampler_info_arr[name];

		VkDescriptorImageInfo* image_info = new VkDescriptorImageInfo[samplers.size()];
		uint32_t i = 0;
		for (auto& s : samplers) {
			image_info[i].sampler = s->getSampler();
			//image_info[i].imageLayout = 0;
			image_info[i].imageView = nullptr;
		}

		VkWriteDescriptorSet writes{};
		writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes.pNext = nullptr;
		writes.dstSet = _descriptor_sets[0];
		writes.dstBinding = info.getBinding();
		writes.dstArrayElement = 0;
		writes.descriptorCount = samplers.size();
		writes.descriptorType = info.getDescriptorType();
		writes.pImageInfo = image_info;
		writes.pBufferInfo = nullptr;
		writes.pTexelBufferView = nullptr;

		write_descriptor_set_arr.push_back(writes);
	}
}

void Pipeline::setWriteDescriptorSet(
	std::unordered_map<std::string, std::vector<Texture2D*>>& texture_arr,
	std::unordered_map<std::string, TextureInfo>& texture_info_arr,
	std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
) {
	for (auto& texture_data : texture_arr) {
		std::string name = texture_data.first;
		std::vector<Texture2D*>& textures = texture_data.second;
		TextureInfo& info = texture_info_arr[name];

		VkDescriptorImageInfo* image_info = new VkDescriptorImageInfo[textures.size()];
		uint32_t i = 0;
		for (auto& tex : textures) {
			image_info[i].sampler = textures[i]->getSampler();
			image_info[i].imageLayout = textures[i]->getImageLayout();
			image_info[i].imageView = (VkImageView)textures[i]->getImageView();
			i++;
		}

		VkWriteDescriptorSet writes{};
		writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writes.pNext = nullptr;
		writes.dstSet = _descriptor_sets[0];
		writes.dstBinding = info.getBinding();
		writes.dstArrayElement = 0;
		writes.descriptorCount = textures.size();
		writes.descriptorType = info.getDescriptorType();
		writes.pImageInfo = image_info;
		writes.pBufferInfo = nullptr;
		writes.pTexelBufferView = nullptr;

		write_descriptor_set_arr.push_back(writes);
	}
}

void Pipeline::updateDescriptorSets() {
	std::vector<VkWriteDescriptorSet> write_descriptor_set_arr;
	setWriteDescriptorSet(
		(std::unordered_map<std::string, Buffer<IGEBufferUsage::uniform_buffer>*>&)_shader->getUniformBuffers(),
		_shader->getUniformBuffersInfo(),
		write_descriptor_set_arr
	);

	setWriteDescriptorSet(
		(std::unordered_map<std::string, Buffer<IGEBufferUsage::storage_buffer>*>&)_shader->getStorageBuffers(),
		_shader->getStorageBuffersInfo(),
		write_descriptor_set_arr
	);

	setWriteDescriptorSet(
		_shader->getSampler(),
		_shader->getSamplerInfo(),
		write_descriptor_set_arr
	);

	setWriteDescriptorSet(
		_shader->getTextures2D(),
		_shader->getTextureInfo(),
		write_descriptor_set_arr
	);

	vkUpdateDescriptorSets(
		_shader->getDevice()->getDevice(),
		write_descriptor_set_arr.size(),
		write_descriptor_set_arr.data(),
		0,
		nullptr
	);

	for (VkWriteDescriptorSet& w : write_descriptor_set_arr) {
		if (w.pBufferInfo != nullptr) {
			delete w.pBufferInfo;
		}
		else if (w.pImageInfo != nullptr) {
			delete w.pImageInfo;
		}
	}
}

void Pipeline::createDescriptorSet() {
	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_binding_arr = setDescriptorSetLayoutBindingArray();
	if (!descriptor_set_layout_binding_arr.size()) {
		return;
	}
	createDescriptorSetLayout(descriptor_set_layout_binding_arr);
	createDescriptorSet(descriptor_set_layout_binding_arr);
	updateDescriptorSets();
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