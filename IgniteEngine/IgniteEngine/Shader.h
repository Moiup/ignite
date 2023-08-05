#pragma once

#include "PhysicalDevice.h"
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

class Shader
{
protected:
	std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;

	std::unordered_map<std::string, ArrayBufferInfo> _uniform_buffers_info;
	std::unordered_map<std::string, ArrayBufferInfo> _storage_buffers_info;

	std::unordered_map<std::string, UniformBuffer*> _uniform_buffers;
	std::unordered_map<std::string, StorageBuffer*> _storage_buffers;

	std::unordered_map<std::string, SamplerInfo> _sampler_info;
	std::unordered_map<std::string, std::vector<Sampler*>> _sampler; 

	std::unordered_map<std::string, TextureInfo> _texture_info;
	std::unordered_map<std::string, std::vector<Texture*>> _textures;

	PhysicalDevice* _gpu;
	VkDevice* _logical_device;

public:
	Shader();

	void setPhysicalDevice(PhysicalDevice* gpu);
	void setLogicalDevice(VkDevice* logical_device);

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
	std::vector<char> virtual readShaderFile(const std::string& path);

protected:
	void createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage);
};
