#pragma once

#include "Module.h"
#include "DispatcherSync.h"

class ComputeShaderTest : public Module
{
private:
	ComputeShader _compute_shader;

	StorageBuffer _input_data;
	StorageBuffer _output_data;
	ComputePipeline _compute_pipeline;
	DispatcherSync _dispatcher_sync;

public:
	ComputeShaderTest();

	void init();
	void start();
	void update();
	void close();
};

