#pragma once

#include "Swapchain.h"
#include "DepthBuffer.h"
#include "GraphicShader.h"

class GraphicsPipeline
{
private:
	VkDevice* _logical_device;
	PhysicalDevice* _physical_device;
	GraphicShader* _shader;
	uint32_t _nb_frame;
	Swapchain* _swapchain;
	DepthBuffer* _depth_buffer;
	
	VkPolygonMode _polygon_mode;
	VkCullModeFlags _cull_mode;
	VkFrontFace _front_face;
	float _line_width;


	std::vector<VkDescriptorSetLayout> _descriptor_set_layout;
	VkDescriptorPool _descriptor_pool;
	std::vector<VkDescriptorSet> _descriptor_sets;
	VkPipelineLayout _pipeline_layout;
	VkPipeline _pipeline;

	std::vector<VkViewport> _viewport_arr;
	std::vector<VkRect2D> _scissor_arr;

public:
	GraphicsPipeline();

	void setLogicalDevice(VkDevice* logical_device);
	void setPhysicalDevice(PhysicalDevice* physical_device);
	void setShader(GraphicShader* shader);
	void setNbFrame(uint32_t nb_frame);
	void setSwapchain(Swapchain* swapchain);
	void setDepthBuffer(DepthBuffer* depth_buffer);

	void setPolygonMode(VkPolygonMode polygon_mode);
	void setCullMode(VkCullModeFlags cull_mode);
	void setFrontFace(VkFrontFace front_face);
	void setLineWidth(float line_width);

	const VkPipeline& getPipeline() const;
	const VkPipelineLayout& getPipelineLayout() const;
	const std::vector<VkDescriptorSet>& getDescriptorSets() const;
	const std::vector<VkViewport>& getViewport() const;
	const std::vector<VkRect2D>& getScissors() const;
	GraphicShader* getShader();

	void create();
	void destroy();

private:
	void setDescriptorSetLayoutBinding(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr, std::unordered_map<std::string, ArrayBufferInfo>& buffer_arr);
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

	void createDescriptorSet();

	void destroyDescriptorSet();
	void destroyDescriptorSetLayout();


	void createPipelineLayout();
	void createPipeline();

	void destroyPipelineLayout();
	void destroyPipeline();
};

