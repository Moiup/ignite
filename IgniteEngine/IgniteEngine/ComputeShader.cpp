#include "ComputeShader.h"

ComputeShader::ComputeShader() :
	Shader::Shader()
{

}
ComputeShader::ComputeShader(std::string compute_shader) :
	ComputeShader::ComputeShader()
{
	read(compute_shader);
}

void ComputeShader::read(std::string compute_shader) {
	createShaderModuleAndStage(
		compute_shader,
		VK_SHADER_STAGE_COMPUTE_BIT
	);
}