#pragma once

#include "Pipeline.h"
#include "ComputeShader.h"

class ComputePipeline : public Pipeline
{

public:
	ComputePipeline();
	ComputePipeline(ComputeShader& shader);

	ComputeShader* getShader();

protected:
	void createPipeline();
};

