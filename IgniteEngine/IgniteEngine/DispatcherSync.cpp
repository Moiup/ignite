#include "DispatcherSync.h"

DispatcherSync::DispatcherSync() :
	Dispatcher::Dispatcher(),
	_compute_pipeline{nullptr},
	_fence{}
{
	;
}

DispatcherSync::DispatcherSync(ComputePipeline* compute_pipeline) :
	Dispatcher::Dispatcher(),
	_compute_pipeline{ compute_pipeline },
	_fence{}
{
	;
}

void DispatcherSync::setComputePipeline(ComputePipeline* compute_pipeline) {
	_compute_pipeline = compute_pipeline;
}

void DispatcherSync::create() {
	Dispatcher::create();
	
	_pipeline_stage_flags = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = 0;

	VkResult result = vkCreateFence(
		_queue->getDevice()->getDevice(),
		&fence_info,
		nullptr,
		&_fence
	);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("DispatcherSync: Error creating fence.");
	}
}

void DispatcherSync::destroy() {
	Dispatcher::destroy();

	vkDestroyFence(
		_queue->getDevice()->getDevice(),
		_fence,
		nullptr
	);
}

void DispatcherSync::dispatch(
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z
) {

	std::vector<VkCommandBuffer>& buffs = _queue->getPendingCommandBuffers();
	CommandBuffer cmd_buf = _queue->allocateCommandBuffer();

	cmd_buf.reset();
	// Start recording
	cmd_buf.begin();

	// Bindings
	cmd_buf.bindPipeline(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		(VkPipeline&)_compute_pipeline->getPipeline()
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

	_compute_pipeline->getShader()->getPushConstants();

	for (auto& pc_info : _compute_pipeline->getShader()->getPushConstantInfo()) {
		std::string name = pc_info.first;
		PushConstantInfo& info = pc_info.second;

		cmd_buf.pushConstants(
			_compute_pipeline->getPipelineLayout(),
			info.getStageFlags(),
			info.getOffset(),
			info.getSize(),
			_compute_pipeline->getShader()->getPushConstants()[name]
		);
	}

	// Dispatch
	cmd_buf.dispatch(
		group_count_x,
		group_count_y,
		group_count_z
	);

	// End recording
	cmd_buf.end();

	_queue->submit();
	_queue->wait();
}