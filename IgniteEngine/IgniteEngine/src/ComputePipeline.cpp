#include "ComputePipeline.h"

ComputePipeline::ComputePipeline() : 
	Pipeline::Pipeline()
{
	;
}

ComputePipeline::ComputePipeline(ComputeShader& shader) :
	Pipeline::Pipeline(shader)
{
	//createPipeline();
	create();
}

ComputeShader* ComputePipeline::getShader() {
	return static_cast<ComputeShader*>(_shader);
}

void ComputePipeline::createPipeline() {
	VkComputePipelineCreateInfo info{};
	
	info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.stage = getShader()->getShaderStages()[0];
	info.layout = _pipeline_layout;
	info.basePipelineHandle = VK_NULL_HANDLE;
	info.basePipelineIndex = 0;

	VkResult vk_result = vkCreateComputePipelines(
		_shader->getDevice()->getDevice(),
		VK_NULL_HANDLE,
		1,
		&info,
		nullptr,
		&_pipeline
	);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Compute Pipeline error: creating the compute pipeline failed.");
	}
}
