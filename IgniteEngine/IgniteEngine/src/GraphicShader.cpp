#include "GraphicShader.h"

GraphicShader::GraphicShader() :
	Shader::Shader(),
	_nb_frame{ 2 },
	_vertex_buffers_info{},
	_index_buffer_info{},
	_vertex_buffers{},
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


void GraphicShader::setNbFrame(uint32_t nb_frame) {
	_nb_frame = nb_frame;
}

uint32_t GraphicShader::getNbFrame() {
	return _nb_frame;
}

void GraphicShader::addVertexBufferInfo(std::string name, uint32_t stride, VkFormat format, uint32_t location) {
	VertexBufferInfo info{};

	info.setStride(stride);
	info.setFormat(format);
	info.setLocation(location);

	_vertex_buffers_info[name] = info;
}

void GraphicShader::addVertexBuffer(std::string name, Buffer<IGEBufferUsage::vertex_buffer>* buffer) {
	_vertex_buffers[name].push_back(buffer);
}

std::unordered_map<std::string, VertexBufferInfo>& GraphicShader::getVertexBuffersInfo() {
	return _vertex_buffers_info;
}

VertexBufferInfo& GraphicShader::getVertexBufferInfo(std::string name) {
	return _vertex_buffers_info[name];
}

std::unordered_map<std::string, std::vector<Buffer<IGEBufferUsage::vertex_buffer>*>>& GraphicShader::getVertexBuffers() {
	return _vertex_buffers;
}

void GraphicShader::addIndexBufferInfo(
	std::string name,
	uint32_t nb_elem,
	VkIndexType index_type
) {
	IndexBufferInfo info{};
	info.setNbElem(nb_elem);
	info.setIndexType(index_type);
	_index_buffer_info[name] = info;
}

std::unordered_map<std::string, IndexBufferInfo>& GraphicShader::getIndexBufferInfo() {
	return _index_buffer_info;
}

IndexBufferInfo& GraphicShader::getIndexBufferInfo(const std::string& name) {
	return _index_buffer_info[name];
}

void GraphicShader::addIndexBuffer(
	std::string name,
	Buffer<IGEBufferUsage::index_buffer>* buffer
) {
	_index_buffers[name].push_back(buffer);
}

std::unordered_map<std::string, std::vector<Buffer<IGEBufferUsage::index_buffer>*>>& GraphicShader::getIndexBuffers() {
	return _index_buffers;
}

void GraphicShader::setPolygonMode(VkPolygonMode polygon_mode) {
	_polygon_mode = polygon_mode;
}

VkPolygonMode GraphicShader::polygonMode() {
	return _polygon_mode;
}

void GraphicShader::setTopology(VkPrimitiveTopology topology) {
	_topology = topology;
}

VkPrimitiveTopology GraphicShader::topology() {
	return _topology;
}

void GraphicShader::read(std::string vertex_shader, std::string fragment_shader) {
	createShaderModuleAndStage(
		vertex_shader, VK_SHADER_STAGE_VERTEX_BIT, shaderc_glsl_vertex_shader
	);
	createShaderModuleAndStage(
		fragment_shader, VK_SHADER_STAGE_FRAGMENT_BIT, shaderc_glsl_fragment_shader
	);
}