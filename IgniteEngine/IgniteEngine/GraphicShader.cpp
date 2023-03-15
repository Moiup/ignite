#include "GraphicShader.h"

GraphicShader::GraphicShader() :
	_logical_device{nullptr},
	_gpu{nullptr},
	_nb_frame{2},
	_vertex_buffers_info{},
	_index_buffer_info{},
	_uniform_buffers{},
	_storage_buffers{}
{ ; }

void GraphicShader::setPhysicalDevice(PhysicalDevice* gpu) {
	_gpu = gpu;
}

void GraphicShader::setLogicalDevice(VkDevice* logical_device) {
	_logical_device = logical_device;
}

void GraphicShader::setNbFrame(uint32_t nb_frame) {
	_nb_frame = nb_frame;
}

uint32_t GraphicShader::getNbFrame() {
	return _nb_frame;
}

void GraphicShader::addVertexBufferInfo(std::string name, uint32_t stride, VkFormat format, uint32_t location) {
	VertexBufferInfo info{};

	info.setStride(stride);
	info.setFormat(format);
	info.setLocation(location);

	_vertex_buffers_info[name] = info;
}

void GraphicShader::addVertexBuffer(std::string name, VertexBuffer* buffer) {
	_vertex_buffers[name].push_back(buffer);
}

std::unordered_map<std::string, VertexBufferInfo>& GraphicShader::getVertexBuffersInfo() {
	return _vertex_buffers_info;
}

VertexBufferInfo& GraphicShader::getVertexBufferInfo(std::string name) {
	return _vertex_buffers_info[name];
}

std::unordered_map<std::string, std::vector<VertexBuffer*>>& GraphicShader::getVertexBuffers() {
	return _vertex_buffers;
}

void GraphicShader::addIndexBufferInfo(std::string name, uint32_t nb_elem) {
	IndexBufferInfo info{};
	info.setNbElem(nb_elem);
	_index_buffer_info[name] = info;
}

void GraphicShader::addIndexBuffer(std::string name, IndexBuffer* buffer) {
	_index_buffers[name].push_back(buffer);
}

std::unordered_map<std::string, std::vector<IndexBuffer*>>& GraphicShader::getIndexBuffers() {
	return _index_buffers;
}

void GraphicShader::addUniformBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags) {
	if (_uniform_buffers_info.count(name)) {
		std::string error = "Error: there already is an Uniform buffer info named " + name +"!";
		throw std::runtime_error(error);
	}

	ArrayBufferInfo info{};
	info.setBinding(binding);
	info.setDescriptorType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	info.setStageFlags(stage_flags);

	_uniform_buffers_info[name] = info;
}

void GraphicShader::addUniformBuffer(std::string name, UniformBuffer* buffer) {
	if (_uniform_buffers.count(name)) {
		std::string error = "Error: there already is an Uniform buffer named " + name +"!";
		throw std::runtime_error(error);
	}

	_uniform_buffers[name] = buffer;
}

std::unordered_map<std::string, ArrayBufferInfo>& GraphicShader::getUniformBuffersInfo() {
	return _uniform_buffers_info;
}

std::unordered_map<std::string, UniformBuffer*>& GraphicShader::getUniformBuffers() {
	return _uniform_buffers;
}

void GraphicShader::addStorageBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags) {
	if (_uniform_buffers_info.count(name)) {
		std::string error = "Error: there already is an Storage buffer info named " + name + "!";
		throw std::runtime_error(error);
	}

	ArrayBufferInfo info{};
	info.setBinding(binding);
	info.setDescriptorType(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
	info.setStageFlags(stage_flags);

	_storage_buffers_info[name] = info;
}

void GraphicShader::addStorageBuffer(std::string name, StorageBuffer* buffer) {
	if (_storage_buffers.count(name)) {
		std::string error = "Error: there already is a Storage buffer named " + name + "!";
		throw std::runtime_error(error);
	}

	_storage_buffers[name] = buffer;
}

std::unordered_map<std::string, ArrayBufferInfo>& GraphicShader::getStorageBuffersInfo() {
	return _storage_buffers_info;
}

std::unordered_map<std::string, StorageBuffer*>& GraphicShader::getStorageBuffers() {
	return _storage_buffers;
}
	
void GraphicShader::readShaderFiles(std::string vertex_shader, std::string fragment_shader) {
	createShaderModuleAndStage(vertex_shader, VK_SHADER_STAGE_VERTEX_BIT);
	createShaderModuleAndStage(fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
}

void GraphicShader::destroy() {
	for (auto& stage : _shader_stages) {
		vkDestroyShaderModule(*_logical_device, stage.module, nullptr);
	}
}

const std::vector<VkPipelineShaderStageCreateInfo>& GraphicShader::getShaderStages() const {
	return _shader_stages;
}

std::vector<char> GraphicShader::readShaderFile(const std::string& path) {
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

void GraphicShader::createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage) {
	std::vector<char> shader_text = readShaderFile(path);

	VkShaderModule shader_module{nullptr};
	VkShaderModuleCreateInfo shader_module_info{};
	shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_info.pNext = nullptr;
	shader_module_info.flags = 0;
	shader_module_info.codeSize = shader_text.size();
	shader_module_info.pCode = reinterpret_cast<const uint32_t*>(shader_text.data());

	VkResult vk_result = vkCreateShaderModule(
		*_logical_device,
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