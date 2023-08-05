#pragma once

#include "Swapchain.h"
#include "Shader.h"
#include "ArrayBuffer.h"

class Pipeline
{
protected:
	VkDevice* _logical_device;
	PhysicalDevice* _physical_device;
	Shader* _shader;

	std::vector<VkDescriptorSetLayout> _descriptor_set_layout;
	VkDescriptorPool _descriptor_pool;
	std::vector<VkDescriptorSet> _descriptor_sets;
	VkPipelineLayout _pipeline_layout;
	VkPipeline _pipeline;

public:
	Pipeline();

	void setLogicalDevice(VkDevice* logical_device);
	void setPhysicalDevice(PhysicalDevice* physical_device);
	void setShader(Shader* shader);

	const VkPipeline& getPipeline() const;
	const VkPipelineLayout& getPipelineLayout() const;
	const std::vector<VkDescriptorSet>& getDescriptorSets() const;
	Shader* getShader();

	virtual void create() = 0;
	virtual void destroy() = 0;

private:
	void setDescriptorSetLayoutBinding(
		std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr,
		std::unordered_map<std::string, ArrayBufferInfo>& buffer_arr
	);
	std::vector<VkDescriptorSetLayoutBinding> setDescriptorSetLayoutBindingArray();
	void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	void createDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	void setWriteDescriptorSet(
		std::unordered_map<std::string, ArrayBuffer*>& buffer_arr,
		std::unordered_map<std::string, ArrayBufferInfo>& buffer_info_arr,
		std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
	);
	void setWriteDescriptorSet(
		std::unordered_map<std::string, std::vector<Sampler*>>& sampler_arr,
		std::unordered_map<std::string, SamplerInfo>& sampler_info_arr,
		std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
	);
	void setWriteDescriptorSet(
		std::unordered_map<std::string, std::vector<Texture*>>& texture_arr,
		std::unordered_map<std::string, TextureInfo>& texture_info_arr,
		std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
	);
	void updateDescriptorSets();

	void destroyDescriptorSetLayout();

protected:
	void createDescriptorSet();
	void destroyDescriptorSet();

	virtual void createPipeline() = 0;
	virtual void destroyPipeline() = 0;
};
