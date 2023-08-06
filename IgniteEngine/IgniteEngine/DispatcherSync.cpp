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
		*_logical_device->getDevice(),
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
		*_logical_device->getDevice(),
		_fence,
		nullptr
	);
}

void DispatcherSync::dispatch(
	uint32_t group_count_x,
	uint32_t group_count_y,
	uint32_t group_count_z
) {
	vkResetFences(
		*_logical_device->getDevice(),
		1,
		&_fence
	);

	_command_buffer.reset();
	// Start recording
	_command_buffer.begin();

	// Bindings
	_command_buffer.bindPipeline(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		(VkPipeline&)_compute_pipeline->getPipeline()
	);
	_command_buffer.bindDescriptorSets(
		VK_PIPELINE_BIND_POINT_COMPUTE,
		_compute_pipeline->getPipelineLayout(),
		0,
		_compute_pipeline->getDescriptorSets().size(),
		_compute_pipeline->getDescriptorSets().data(),
		0,
		nullptr
	);

	// Dispatch
	_command_buffer.dispatch(
		group_count_x,
		group_count_y,
		group_count_z
	);

	// End recording
	_command_buffer.end();

	_logical_device->getQueue("compute_queue")->submit(
		0, nullptr,
		nullptr,
		1, _command_buffer.getCommandBuffer(),
		0, nullptr,
		&_fence
	);

	VkResult result = vkWaitForFences(
		*_logical_device->getDevice(),
		1, &_fence,
		VK_TRUE,
		UINT64_MAX
	);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("DispatcherSync::dispatch: Error while waiting for fences to finish.");
	}
}