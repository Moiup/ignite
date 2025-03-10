#include "GraphicShader.h"

GraphicShader::GraphicShader() :
	Shader::Shader(),
	_index_buffer_info{}
{ ; }

GraphicShader::GraphicShader(
	Device& device,
	const std::string& vertex_shader,
	const std::string& fragment_shader
) : 
	Shader::Shader(device)
{
	read(
		vertex_shader, fragment_shader
	);
}

GraphicShader::GraphicShader(const GraphicShader& shader) {
	*this = shader;
}

GraphicShader::GraphicShader(GraphicShader&& shader) {
	*this = std::move(shader);
}

GraphicShader& GraphicShader::operator=(const GraphicShader& shader) {
	Shader::operator=(shader);
	_vertex_input_descs = shader._vertex_input_descs;
	_index_buffer_info = shader._index_buffer_info;

	return *this;
}

GraphicShader& GraphicShader::operator=(GraphicShader&& shader) {
	Shader::operator=(shader);
	_vertex_input_descs = std::move(shader)._vertex_input_descs;
	_index_buffer_info = std::move(shader)._index_buffer_info;

	return *this;
}

void GraphicShader::configureVertexBuffer(
	const std::string& name,
	uint32_t location,
	VkFormat format,
	uint32_t stride,
	uint32_t offset
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