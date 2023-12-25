#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
private:


public:
	ComputeShader();
	ComputeShader(std::string compute_shader);

	void read(std::string compute_shader);
};

