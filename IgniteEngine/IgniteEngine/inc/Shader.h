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

	std::unordered_map<std::string, PushConstantInfo> _push_constant_info;

	std::unordered_map<std::string, VkDescriptorSetLayoutBinding> _desc_layout_bindings;

	std::unordered_map<std::string, void*> _push_constants;

	Device* _device;

public:
	Shader();

	void setDevice(Device* device);
	Device* getDevice();


	void addPushConstantInfo(std::string name, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size);
	void addPushConstant(std::string name, void* push_constant);
	std::unordered_map<std::string, PushConstantInfo>& getPushConstantInfo();
	std::unordered_map<std::string, void*>& getPushConstants();


	void addDescriptorLayout(
		std::string name,
		uint32_t binding,
		VkShaderStageFlags stage_flags,
		VkDescriptorType type,
		uint32_t descriptor_count
	);
	const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& getBuffersInfo() const;
	
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

	void destroy();

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

protected:
	void createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage, shaderc_shader_kind shader_kind);
};

