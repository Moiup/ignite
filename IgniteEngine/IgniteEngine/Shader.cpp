#include "Shader.h"

Shader::Shader() :
	_uniform_buffers_info{},
	_storage_buffers_info{},
	_uniform_buffers{},
	_storage_buffers{},
	_sampler_info{},
	_sampler{},
	_texture_info{},
	_gpu{nullptr},
	_logical_device{nullptr}
{
	;
}

void Shader::setPhysicalDevice(PhysicalDevice* gpu) {
	_gpu = gpu;
}

void Shader::setLogicalDevice(LogicalDevice* logical_device) {
	_logical_device = logical_device;
}

void Shader::addUniformBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags) {
	if (_uniform_buffers_info.count(name)) {
		std::string error = "Error: there already is an Uniform buffer info named " + name + "!";
		throw std::runtime_error(error);
	}

	ArrayBufferInfo info{};
	info.setBinding(binding);
	info.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	info.setStageFlags(stage_flags);

	_uniform_buffers_info[name] = info;
}

void Shader::addUniformBuffer(std::string name, UniformBuffer* buffer) {
	if (_uniform_buffers.count(name)) {
		std::string error = "Error: there already is an Uniform buffer named " + name + "!";
		throw std::runtime_error(error);
	}

	_uniform_buffers[name] = buffer;
}

std::unordered_map<std::string, ArrayBufferInfo>& Shader::getUniformBuffersInfo() {
	return _uniform_buffers_info;
}

std::unordered_map<std::string, UniformBuffer*>& Shader::getUniformBuffers() {
	return _uniform_buffers;
}

void Shader::addStorageBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags) {
	if (_uniform_buffers_info.count(name)) {
		std::string error = "Error: there already is a Storage buffer info named " + name + "!";
		throw std::runtime_error(error);
	}

	ArrayBufferInfo info{};
	info.setBinding(binding);
	info.setDescriptorType(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	info.setStageFlags(stage_flags);

	_storage_buffers_info[name] = info;
}

void Shader::addStorageBuffer(std::string name, StorageBuffer* buffer) {
	if (_storage_buffers.count(name)) {
		std::string error = "Error: there already is a Storage buffer named " + name + "!";
		throw std::runtime_error(error);
	}

	_storage_buffers[name] = buffer;
}

std::unordered_map<std::string, ArrayBufferInfo>& Shader::getStorageBuffersInfo() {
	return _storage_buffers_info;
}

std::unordered_map<std::string, StorageBuffer*>& Shader::getStorageBuffers() {
	return _storage_buffers;
}

void Shader::addSamplerInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags) {
	if (_sampler_info.count(name)) {
		std::string error = "Error: there already is a sampler info named " + name + "!";
		throw std::runtime_error(error);
	}

	SamplerInfo info{};
	info.setBinding(binding);
	info.setDescriptorType(VK_DESCRIPTOR_TYPE_SAMPLER);
	info.setStageFlags(stage_flags);

	_sampler_info[name] = info;
}

void Shader::addSampler(std::string name, Sampler* sampler) {
	_sampler[name].push_back(sampler);
}

std::unordered_map<std::string, SamplerInfo>& Shader::getSamplerInfo() {
	return _sampler_info;
}

std::unordered_map<std::string, std::vector<Sampler*>>& Shader::getSampler() {
	return _sampler;
}


void Shader::addTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags, uint32_t descriptor_count) {
	if (_texture_info.count(name)) {
		std::string error = "Error: there already is a texture info named " + name + "!";
		throw std::runtime_error(error);
	}

	TextureInfo info{};
	info.setBinding(binding);
	info.setDescriptorType(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
	info.setStageFlags(stage_flags);
	info.setDescriptorCount(descriptor_count);

	_texture_info[name] = info;
}

void Shader::addTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags) {
	addTextureInfo(name, binding, stage_flags, 1);
}

void Shader::addTexture(std::string name, Texture* texture) {
	_textures[name].push_back(texture);
}


void Shader::addTexture(std::string name, std::vector<Texture*>& textures) {
	_textures[name].insert(
		_textures[name].end(),
		textures.begin(),
		textures.end()
	);
}

std::unordered_map<std::string, TextureInfo>& Shader::getTextureInfo() {
	return _texture_info;
}

std::unordered_map<std::string, std::vector<Texture*>>& Shader::getTexture() {
	return _textures;
}

void Shader::destroy() {
	for (auto& stage : _shader_stages) {
		vkDestroyShaderModule(*_logical_device->getDevice(), stage.module, nullptr);
	}
}

const std::vector<VkPipelineShaderStageCreateInfo>& Shader::getShaderStages() const {
	return _shader_stages;
}

std::vector<char> Shader::readShaderFile(const std::string& path) {
	size_t file_size{};
	std::vector<char> buffer;

	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Error: failed to open file: " + path);
	}

	file_size = (size_t)file.tellg();
	buffer.resize(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

void Shader::createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage) {
	std::vector<char> shader_text = readShaderFile(path);

	VkShaderModule shader_module{ nullptr };
	VkShaderModuleCreateInfo shader_module_info{};
	shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_info.pNext = nullptr;
	shader_module_info.flags = 0;
	shader_module_info.codeSize = shader_text.size();
	shader_module_info.pCode = reinterpret_cast<const uint32_t*>(shader_text.data());

	VkResult vk_result = vkCreateShaderModule(
		*_logical_device->getDevice(),
		&shader_module_info,
		nullptr,
		&shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating shader module!");
	}

	VkPipelineShaderStageCreateInfo pipeline{};
	pipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pipeline.pNext = nullptr;
	pipeline.flags = 0;
	pipeline.stage = stage;
	pipeline.module = shader_module;
	pipeline.pName = "main";
	pipeline.pSpecializationInfo = nullptr;

	_shader_stages.push_back(pipeline);
}