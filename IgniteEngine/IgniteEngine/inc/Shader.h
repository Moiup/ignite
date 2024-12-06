#pragma once

#include "PhysicalDevice.h"
#include "Device.h"
#include "Buffer.h"
#include "PushConstantInfo.h"
#include "VertexBufferInfo.h"
#include "IndexBufferInfo.h"
#include "ArrayBufferInfo.h"
#include "Sampler.h"
#include "SamplerInfo.h"
#include "TextureInfo.h"
#include "Texture2D.h"

#include <unordered_map>
#include <fstream>
#include <string>

// #include <shaderc/shaderc.hpp>

#include "shaderc/shaderc.hpp"

class Shader
{
protected:
	std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;

	VkPushConstantRange _push_constant_range;

	std::unordered_map<std::string, VkDescriptorSetLayoutBinding> _desc_layout_bindings;

	Device* _device;

	int32_t* _shared_count;

public:
	Shader();
	Shader(Device& device);
	Shader(const Shader& shader);
	~Shader();

	Shader& operator=(const Shader& shader);

	void setDevice(Device* device);
	Device* getDevice();


	void configurePushConstant(
		VkShaderStageFlags stage_flags,
		uint32_t offset,
		uint32_t size
	);
	const VkPushConstantRange& getPushConstantRange() const;

	const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& getDescLayoutBindings() const;
	
	void configureUniformBuffer(
		const std::string& name,
		uint32_t binding,
		VkShaderStageFlags stage_flags
	);
	void configureStorageBuffer(
		const std::string& name,
		uint32_t binding,
		VkShaderStageFlags stage_flags
	);
	void configureSampler(
		const std::string& name,
		uint32_t binding,
		VkShaderStageFlags stage_flags
	);
	void configureTexture2D(
		const std::string& name,
		uint32_t binding,
		VkShaderStageFlags stage_flags,
		int32_t descriptor_count = 1
	);
	void configureTexture2DCombined(
		const std::string& name,
		uint32_t binding,
		VkShaderStageFlags stage_flags,
		int32_t descriptor_count = 1
	);
	void configureStorageTexture2D(
		const std::string& name,
		uint32_t binding,
		VkShaderStageFlags stage_flags,
		uint32_t descriptor_count = 1
	);


	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

private:
	std::string virtual readShaderFile(const std::string& path);
	std::vector<uint32_t> compile(const std::string& glsl, const std::string& path, shaderc_shader_kind shader_kind);
	void configureDescLayoutBindings(
		const std::string& name,
		uint32_t binding,
		VkDescriptorType descriptor_type,
		VkShaderStageFlags stage_flags,
		uint32_t descriptor_count
	);

	void destroy();

protected:
	void createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage, shaderc_shader_kind shader_kind);
};

