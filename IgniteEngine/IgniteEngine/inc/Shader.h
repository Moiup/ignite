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

	std::unordered_map<std::string, ArrayBufferInfo> _uniform_buffers_info;
	std::unordered_map<std::string, ArrayBufferInfo> _storage_buffers_info;

	std::unordered_map<std::string, void*> _push_constants;

	std::unordered_map<std::string, Buffer<IGEBufferUsage::uniform_buffer>*> _uniform_buffers;
	std::unordered_map<std::string, Buffer<IGEBufferUsage::storage_buffer>*> _storage_buffers;

	std::unordered_map<std::string, SamplerInfo> _sampler_info;
	std::unordered_map<std::string, std::vector<Sampler*>> _sampler; 

	std::unordered_map<std::string, TextureInfo> _texture_info;
	std::unordered_map<std::string, std::vector<Texture2D*>> _textures;

	Device* _device;

public:
	Shader();

	void setDevice(Device* device);
	Device* getDevice();

	void addPushConstantInfo(std::string name, VkShaderStageFlags stage_flags, uint32_t offset, uint32_t size);
	void addPushConstant(std::string name, void* push_constant);
	std::unordered_map<std::string, PushConstantInfo>& getPushConstantInfo();
	std::unordered_map<std::string, void*>& getPushConstants();

	void addUniformBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addUniformBuffer(std::string name, Buffer<IGEBufferUsage::uniform_buffer>* buffer);
	std::unordered_map<std::string, ArrayBufferInfo>& getUniformBuffersInfo();
	std::unordered_map<std::string, Buffer<IGEBufferUsage::uniform_buffer>*>& getUniformBuffers();

	void addStorageBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addStorageBuffer(std::string name, Buffer<IGEBufferUsage::storage_buffer>* buffer);
	std::unordered_map<std::string, ArrayBufferInfo>& getStorageBuffersInfo();
	std::unordered_map<std::string, Buffer<IGEBufferUsage::storage_buffer>*>& getStorageBuffers();

	void addSamplerInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addSampler(std::string name, Sampler* sampler);
	std::unordered_map<std::string, SamplerInfo>& getSamplerInfo();
	std::unordered_map<std::string, std::vector<Sampler*>>& getSampler();

	void addTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags, uint32_t descriptor_count);
	void addTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addStorageTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags, uint32_t descriptor_count);
	void addStorageTextureInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	//void addStorageTexture(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addTexture2D(std::string name, Texture2D* texture);
	void addTexture2D(std::string name, std::vector<Texture2D*>& textures);
	std::unordered_map<std::string, TextureInfo>& getTextureInfo();
	std::unordered_map<std::string, std::vector<Texture2D*>>& getTextures2D();

	void destroy();

	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

private:
	std::string virtual readShaderFile(const std::string& path);
	std::vector<uint32_t> compile(const std::string& glsl, const std::string& path, shaderc_shader_kind shader_kind);

protected:
	void createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage, shaderc_shader_kind shader_kind);
};

