#include "Shader.h"

Shader::Shader() :
	_device{nullptr}
{
	;
}

void Shader::setDevice(Device* device) {
	_device = device;
}

Device* Shader::getDevice() {
	return _device;
}

void Shader::addPushConstantInfo(std::string name, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size) {
	if (_push_constant_info.count(name)) {
		std::string error = "Error: there already is a Push Constant info named " + name + "!";
		throw std::runtime_error(error);
	}

	PushConstantInfo info{};
	info.setStageFlags(stage_flags);
	info.setOffset(offset);
	info.setSize(size);

	_push_constant_info[name] = info;
}

void Shader::addPushConstant(std::string name, void* push_constant) {
	_push_constants[name] = push_constant;
}

std::unordered_map<std::string, PushConstantInfo>& Shader::getPushConstantInfo() {
	return _push_constant_info;
}

std::unordered_map<std::string, void*>& Shader::getPushConstants() {
	return _push_constants;
}

void Shader::configureUniformBuffer(
	const std::string& name,
	uint32_t binding,
	VkShaderStageFlags stage_flags
) {
	configureDescLayoutBindings(
		name,
		binding,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		stage_flags,
		1
	);
}

void Shader::configureStorageBuffer(
	const std::string& name,
	uint32_t binding,
	VkShaderStageFlags stage_flags
) {
	configureDescLayoutBindings(
		name,
		binding,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		stage_flags,
		1
	);
}

void Shader::configureSampler(
	const std::string& name,
	uint32_t binding,
	VkShaderStageFlags stage_flags
) {
	configureDescLayoutBindings(
		name,
		binding,
		VK_DESCRIPTOR_TYPE_SAMPLER,
		stage_flags,
		1
	);
}

void Shader::configureTexture2D(
	const std::string& name,
	uint32_t binding,
	VkShaderStageFlags stage_flags,
	int32_t descriptor_count = 1
) {
	configureDescLayoutBindings(
		name,
		binding,
		VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		stage_flags,
		descriptor_count
	);
}

void Shader::configureTexture2DCombined(
	const std::string& name,
	uint32_t binding,
	VkShaderStageFlags stage_flags,
	int32_t descriptor_count = 1
) {
	configureDescLayoutBindings(
		name,
		binding,
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		stage_flags,
		descriptor_count
	);
}


void Shader::configureStorageTexture2D(
	const std::string& name,
	uint32_t binding,
	VkShaderStageFlags stage_flags,
	uint32_t descriptor_count = 1
) {
	configureDescLayoutBindings(
		name,
		binding,
		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		stage_flags,
		descriptor_count
	);
}


void Shader::destroy() {
	for (auto& stage : _shader_stages) {
		vkDestroyShaderModule(
			_device->getDevice(),
			stage.module,
			nullptr
		);
	}
}

const std::vector<VkPipelineShaderStageCreateInfo>& Shader::getShaderStages() const {
	return _shader_stages;
}

std::string Shader::readShaderFile(const std::string& path) {
	std::ifstream file(path);
	std::string content;

	file.seekg(0, std::ios::end);
	content.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	content.assign(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);

	return content;
}

std::vector<uint32_t> Shader::compile(const std::string& glsl, const std::string& path, shaderc_shader_kind shader_kind) {
	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	//options.AddMacroDefinition("MY_DEFINE", "1");

	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(glsl, shader_kind, path.c_str());

	if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << "Error compiling shader \"" << path << "\"" << std::endl;
		std::cerr << result.GetErrorMessage() << std::endl;
		throw std::runtime_error("Error compiling shader");
	}

	return {result.cbegin(), result.cend()};
}

void Shader::configureDescLayoutBindings(
	const std::string& name,
	uint32_t binding,
	VkDescriptorType descriptor_type,
	VkShaderStageFlags stage_flags,
	uint32_t descriptor_count
) {
	VkDescriptorSetLayoutBinding info{};
	info.binding = binding;
	info.descriptorType = descriptor_type;
	info.descriptorCount = descriptor_count;
	info.stageFlags = stage_flags;
	info.pImmutableSamplers = nullptr;

	_desc_layout_bindings[name] = info;
}

void Shader::createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage, shaderc_shader_kind shader_kind) {
	std::string glsl = readShaderFile(path);
	
	std::vector<uint32_t> shader_text = compile(glsl, path, shader_kind);

	VkShaderModule shader_module{ nullptr };
	VkShaderModuleCreateInfo shader_module_info{};
	shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_info.pNext = nullptr;
	shader_module_info.flags = 0;
	shader_module_info.codeSize = shader_text.size() * sizeof(uint32_t);
	shader_module_info.pCode = shader_text.data();

	VkResult vk_result = vkCreateShaderModule(
		_device->getDevice(),
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