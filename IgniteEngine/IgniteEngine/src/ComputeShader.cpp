#include "ComputeShader.h"

ComputeShader::ComputeShader() :
	Shader::Shader()
{

}
ComputeShader::ComputeShader(
	Device& device,
	std::string compute_shader
) :
	Shader::Shader(device)
{
	read(compute_shader);
}

void ComputeShader::read(std::string compute_shader) {
	createShaderModuleAndStage(
		compute_shader,
		VK_SHADER_STAGE_COMPUTE_BIT,
		shaderc_glsl_compute_shader
	);
}