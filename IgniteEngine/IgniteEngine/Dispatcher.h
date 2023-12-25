#pragma once

#include "CommandBuffer.h"
#include "DefaultConf.h"
#include "ComputePipeline.h"

class Dispatcher
{
protected:
	CommandPool* _command_pool;
	LogicalDevice* _logical_device;

	CommandBuffer _command_buffer;

	std::vector<VkPipelineStageFlags> _pipeline_stage_flags;

public:
	Dispatcher();
	Dispatcher(
		LogicalDevice* logical_device,
		CommandPool* command_pool
	);

	void setLogicalDevice(LogicalDevice* logical_device);
	void setCommandPool(CommandPool* command_pool);

	void create();
	void destroy();

	virtual void dispatch(
		uint32_t group_count_x,
		uint32_t group_count_y,
		uint32_t group_count_z
	) = 0;
};

