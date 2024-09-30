#pragma once

#include "Dispatcher.h"

class DefaultDispatcher: public Dispatcher
{
private:
	ComputePipeline* _compute_pipeline;

public:
	DefaultDispatcher();
	DefaultDispatcher(ComputePipeline* compute_pipeline);

	void setComputePipeline(ComputePipeline* compute_pipeline);

	void create();
	void destroy();

	void dispatch(
		uint32_t group_count_x,
		uint32_t group_count_y,
		uint32_t group_count_z
	);
};

