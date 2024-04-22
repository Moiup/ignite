#pragma once

#include "CommandBuffer.h"
#include "DefaultConf.h"
#include "ComputePipeline.h"

class Dispatcher
{
protected:
	Queue* _queue;
	Device* _device;

	CommandBuffer _command_buffer;

	std::vector<VkPipelineStageFlags> _pipeline_stage_flags;

public:
	Dispatcher();

	void setDevice(Device* device);
	void setQueue(Queue* queue);

	void create();
	void destroy();

	virtual void dispatch(
		uint32_t group_count_x,
		uint32_t group_count_y,
		uint32_t group_count_z
	) = 0;
};

