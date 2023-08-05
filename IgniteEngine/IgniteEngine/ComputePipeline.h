#pragma once

#include "Pipeline.h"
#include "ComputeShader.h"

class ComputePipeline : public Pipeline
{

public:
	ComputePipeline();

	ComputeShader* getShader();

	void create();
	void destroy();

protected:
	void CreatePipeline();
	void destroyPipeline();
};

