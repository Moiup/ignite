#include "DefaultDispatcher.h"

DefaultDispatcher::DefaultDispatcher() :
	Dispatcher::Dispatcher(),
	_compute_pipeline{nullptr}
{
	;
}

DefaultDispatcher::DefaultDispatcher(ComputePipeline* compute_pipeline) :
	Dispatcher::Dispatcher(),
	_compute_pipeline{ compute_pipeline }
{
	;
}

void DefaultDispatcher::setComputePipeline(ComputePipeline* compute_pipeline) {
	_compute_pipeline = compute_pipeline;
}

void DefaultDispatcher::create() {
	;
}

void DefaultDispatcher::destroy() {
	;
}

void DefaultDispatcher::dispatch(
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z
) {

	std::vector<VkCommandBuffer>& buffs = _queue->getPendingCommandBuffers();
	CommandBuffer cmd_buf = _queue->newCommandBuffer();

	cmd_buf.reset();
	// Start recording
	cmd_buf.begin();

	// Bindings
	cmd_buf.bindPipeline(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		_compute_pipeline->getPipeline()
	);
	cmd_buf.bindDescriptorSets(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		_compute_pipeline->getPipelineLayout(),
		0,
		_compute_pipeline->getDescriptorSets().size(),
		_compute_pipeline->getDescriptorSets().data(),
		0,
		nullptr
	);

	const VkPushConstantRange& pc_range = _compute_pipeline->getShader()->getPushConstantRange();

	cmd_buf.pushConstants(
		_compute_pipeline->getPipelineLayout(),
		pc_range.stageFlags,
		pc_range.offset,
		pc_range.size,
		_compute_pipeline->getPushConstants()
	);


	// Dispatch
	cmd_buf.dispatch(
		group_count_x,
		group_count_y,
		group_count_z
	);

	// End recording
	cmd_buf.end();
}
