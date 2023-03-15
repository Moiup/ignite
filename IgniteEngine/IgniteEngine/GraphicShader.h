#pragma once

#include "PhysicalDevice.h"
#include "VertexBufferInfo.h"
#include "VertexBuffer.h"
#include "IndexBufferInfo.h"
#include "IndexBuffer.h"
#include "ArrayBufferInfo.h"
#include "StorageBuffer.h"
#include "UniformBuffer.h"

#include <unordered_map>
#include <fstream>
#include <string>

class GraphicShader
{
private:
	std::vector<VkPipelineShaderStageCreateInfo> _shader_stages;

	std::unordered_map<std::string, VertexBufferInfo> _vertex_buffers_info;
	std::unordered_map<std::string, IndexBufferInfo> _index_buffer_info;

	std::unordered_map<std::string, std::vector<VertexBuffer*>> _vertex_buffers;
	std::unordered_map<std::string, std::vector<IndexBuffer*>> _index_buffers;

	std::unordered_map<std::string, ArrayBufferInfo> _uniform_buffers_info;
	std::unordered_map<std::string, ArrayBufferInfo> _storage_buffers_info;

	std::unordered_map<std::string, UniformBuffer*> _uniform_buffers;
	std::unordered_map<std::string, StorageBuffer*> _storage_buffers;

	PhysicalDevice* _gpu;
	VkDevice* _logical_device;
	uint32_t _nb_frame;

public:
	GraphicShader();

	void setPhysicalDevice(PhysicalDevice* gpu);
	void setLogicalDevice(VkDevice* logical_device);

	void setNbFrame(uint32_t nb_frame);
	uint32_t getNbFrame();

	void addVertexBufferInfo(std::string name, uint32_t stride, VkFormat format, uint32_t location);
	std::unordered_map<std::string, VertexBufferInfo>& getVertexBuffersInfo();
	VertexBufferInfo& getVertexBufferInfo(std::string name);
	void addVertexBuffer(std::string name, VertexBuffer* buffer);
	std::unordered_map<std::string, std::vector<VertexBuffer*>>& getVertexBuffers();

	void addIndexBufferInfo(std::string name, uint32_t nb_elem);
	void addIndexBuffer(std::string name, IndexBuffer* buffer);
	std::unordered_map<std::string, std::vector<IndexBuffer*>>& getIndexBuffers();

	void addUniformBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addUniformBuffer(std::string name, UniformBuffer* buffer);
	std::unordered_map<std::string, ArrayBufferInfo>& getUniformBuffersInfo();
	std::unordered_map<std::string, UniformBuffer*>& getUniformBuffers();

	void addStorageBufferInfo(std::string name, uint32_t binding, VkShaderStageFlags stage_flags);
	void addStorageBuffer(std::string name, StorageBuffer* buffer);
	std::unordered_map<std::string, ArrayBufferInfo>& getStorageBuffersInfo();
	std::unordered_map<std::string, StorageBuffer*>& getStorageBuffers();

	void readShaderFiles(std::string vertex_shader, std::string fragment_shader);

	void destroy();

	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const;

private:
	std::vector<char> readShaderFile(const std::string& path);
	void createShaderModuleAndStage(const std::string& path, VkShaderStageFlagBits stage);
};

