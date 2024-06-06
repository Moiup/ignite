#pragma once

#include "CommandBuffer.h"
#include "DefaultConf.h"
#include "ComputePipeline.h"

class Dispatcher
{
protected:
	Queue* _queue;

	std::vector<VkPipelineStageFlags> _pipeline_stage_flags;

public:
	Dispatcher();

	void setQueue(Queue* queue);

	virtual void create() = 0;
	virtual void destroy() = 0;

	virtual void dispatch(
		uint32_t group_count_x,
		uint32_t group_count_y,
		uint32_t group_count_z
	) = 0;


	void submit();
	void wait();

	void submitAndWait();
};

