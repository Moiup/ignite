#pragma once

#include "PhysicalDevice.h"
#include "PushConstantInfo.h"
#include "VertexBufferInfo.h"
#include "VertexBuffer.h"
#include "IndexBufferInfo.h"
#include "IndexBuffer.h"
#include "ArrayBufferInfo.h"
#include "StorageBuffer.h"
#include "UniformBuffer.h"
#include "Sampler.h"
#include "SamplerInfo.h"
#include "TextureInfo.h"
#include "Texture.h"

#include <unordered_map>
#include <fstream>
#include <string>

#include <shaderc/shaderc.hpp>

class Shader
{
protected:
	std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;

	std::unordered_map<std::string, PushConstantInfo> _push_constant_info;

	std::unordered_map<std::string, ArrayBufferInfo> _uniform_buffers_info;
	std::unordered_map<std::string, ArrayBufferInfo> _storage_buffers_info;

	std::unordered_map<std::string, void*> _push_constants;

	std::unordered_map<std::string, UniformBuffer*> _uniform_buffers;
	std::unordered_map<std::string, StorageBuffer*> _storage_buffers;

	std::unordered_map<std::string, SamplerInfo> _sampler_info;
	std::unordered_map<std::string, std::vector<Sampler*>> _sampler; 

	std::unordered_map<std::string, TextureInfo> _texture_info;
	std::unordered_map<std::string, std::vector<Texture*>> _textures;

	PhysicalDevice* _gpu;
	LogicalDevice* _logical_device;

public:
	Shader();

	void setPhysicalDevice(PhysicalDevice* gpu);
	void setLogicalDevice(LogicalDevice* logical_device);

	void addPushConstantInfo(std::string name, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size);
	void addPushConstant(std::string name, void* push_constant);
	std::unordered_map<std::string, PushConstantInfo>& getPushConstantInfo();
	std::unordered_map<std::string, void*>& getPushConstants();

	void addUniformBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addUniformBuffer(std::string name, UniformBuffer* buffer);
	std::unordered_map<std::string, ArrayBufferInfo>& getUniformBuffersInfo();
	std::unordered_map<std::string, UniformBuffer*>& getUniformBuffers();

	void addStorageBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addStorageBuffer(std::string name, StorageBuffer* buffer);
	std::unordered_map<std::string, ArrayBufferInfo>& getStorageBuffersInfo();
	std::unordered_map<std::string, StorageBuffer*>& getStorageBuffers();

	void addSamplerInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addSampler(std::string name, Sampler* sampler);
	std::unordered_map<std::string, SamplerInfo>& getSamplerInfo();
	std::unordered_map<std::string, std::vector<Sampler*>>& getSampler();

	void addTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags, uint32_t descriptor_count);
	void addTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addTexture(std::string name, Texture* texture);
	void addTexture(std::string name, std::vector<Texture*>& textures);
	std::unordered_map<std::string, TextureInfo>& getTextureInfo();
	std::unordered_map<std::string, std::vector<Texture*>>& getTexture();

	void destroy();

	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

private:
	std::string virtual readShaderFile(const std::string& path);
	std::vector<uint32_t> compile(const std::string& glsl, const std::string& path, shaderc_shader_kind shader_kind);

protected:
	void createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage, shaderc_shader_kind shader_kind);
};

