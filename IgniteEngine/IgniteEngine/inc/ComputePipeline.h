#pragma once

#include "Pipeline.h"
#include "ComputeShader.h"

class ComputePipeline : public Pipeline
{

public:
	ComputePipeline();

	ComputeShader* getShader();

protected:
	void createPipeline();
	void destroyPipeline();
};

