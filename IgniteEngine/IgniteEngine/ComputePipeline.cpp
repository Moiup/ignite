#include "ComputePipeline.h"

ComputePipeline::ComputePipeline() : 
	Pipeline::Pipeline()
{
	;
}

ComputeShader* ComputePipeline::getShader() {
	return (ComputeShader*)_shader;
}

void ComputePipeline::create() {
	createDescriptorSet();
	createPipeline();
}

void ComputePipeline::destroy() {
	destroyPipeline();
	destroyDescriptorSet();
}

void ComputePipeline::createPipeline() {
	VkComputePipelineCreateInfo info{};
	
	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = getShader()->getShaderStages()[0];
	info.layout = _pipeline_layout;
	info.basePipelineHandle = _pipeline;
	info.basePipelineIndex = 0;

	VkResult vk_result = vkCreateComputePipelines(
		*_logical_device,
		nullptr,
		1,
		&info,
		nullptr,
		&_pipeline
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Compute Pipeline error: creating the compute pipeline failed.");
	}
}

void ComputePipeline::destroyPipeline() {
	vkDestroyPipeline(
		*_logical_device,
		_pipeline,
		nullptr
	);
}