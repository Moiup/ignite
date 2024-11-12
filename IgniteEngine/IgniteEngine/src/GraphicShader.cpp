#include "GraphicShader.h"

GraphicShader::GraphicShader() :
	Shader::Shader(),
	_index_buffer_info{},
	_index_buffers{}
{ ; }

GraphicShader::GraphicShader(
	std::string vertex_shader,
	std::string fragment_shader
) : 
	GraphicShader::GraphicShader()
{
	read(
		vertex_shader, fragment_shader
	);
}

void GraphicShader::configureVertexBuffer(
	const std::string& name,
	uint32_t location,
	VkFormat format,
	uint32_t stride = 0,
	uint32_t offset = 0
) {
	VkVertexInputBindingDescription binding_desc{};
	binding_desc.binding = _vertex_input_descs.size();
	binding_desc.stride = stride;
	binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attrib_desc{};
	attrib_desc.location = location;
	attrib_desc.binding = binding_desc.binding;
	attrib_desc.format = format;
	attrib_desc.offset = offset;

	VertexInputDescription input_desc{};
	input_desc.binding_desc = binding_desc;
	input_desc.attribute_desc = attrib_desc;

	_vertex_input_descs[name] = input_desc;
}

const VertexInputDescription& GraphicShader::getVertexBufferDescription(const std::string& name) const {
	return _vertex_input_descs.at(name);
}

const std::unordered_map<std::string, VertexInputDescription>& GraphicShader::getVertexBufferDescription() const {
	return _vertex_input_descs;
}

void GraphicShader::configureIndexBuffer(
	uint32_t nb_elem,
	VkIndexType index_type
) {
	_index_buffer_info.setNbElem(nb_elem);
	_index_buffer_info.setIndexType(index_type);
}

const IndexBufferInfo& GraphicShader::getIndexBufferInfo() const {
	return _index_buffer_info;
}

void GraphicShader::read(std::string vertex_shader, std::string fragment_shader) {
	createShaderModuleAndStage(
		vertex_shader, VK_SHADER_STAGE_VERTEX_BIT, shaderc_glsl_vertex_shader
	);
	createShaderModuleAndStage(
		fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader
	);
}