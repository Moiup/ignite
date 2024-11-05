#pragma once

#include "Buffer.h"
#include "Shader.h"
#include <unordered_map>
#include <fstream>
#include <string>

class GraphicShader : public Shader
{
private:
	std::unordered_map<std::string, VertexBufferInfo> _vertex_buffers_info;
	std::unordered_map<std::string, IndexBufferInfo> _index_buffer_info;

	std::unordered_map<std::string, std::vector<Buffer<IGEBufferUsage::vertex_buffer>*>> _vertex_buffers;
	std::unordered_map<std::string, std::vector<Buffer<IGEBufferUsage::index_buffer>*>> _index_buffers;

	uint32_t _nb_frame;
	VkPolygonMode _polygon_mode{ VK_POLYGON_MODE_FILL };
	VkPrimitiveTopology _topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };

public:
	GraphicShader();
	GraphicShader(std::string vertex_shader, std::string fragment_shader);

	void setNbFrame(uint32_t nb_frame);
	uint32_t getNbFrame();

	void addVertexBufferInfo(
		std::string name,
		uint32_t stride,
		VkFormat format,
		uint32_t location
	);
	std::unordered_map<std::string, VertexBufferInfo>& getVertexBuffersInfo();
	VertexBufferInfo& getVertexBufferInfo(std::string name);
	void addVertexBuffer(
		std::string name,
		Buffer<IGEBufferUsage::vertex_buffer>* buffer
	);
	std::unordered_map<std::string, std::vector<Buffer<IGEBufferUsage::vertex_buffer>*>>& getVertexBuffers();

	void addIndexBufferInfo(
		std::string name,
		uint32_t nb_elem,
		VkIndexType index_type = VK_INDEX_TYPE_UINT32
	);
	std::unordered_map<std::string, IndexBufferInfo>& getIndexBufferInfo();
	IndexBufferInfo& getIndexBufferInfo(const std::string& name);
	void addIndexBuffer(
		std::string name,
		Buffer<IGEBufferUsage::index_buffer>* buffer
	);
	std::unordered_map<std::string, std::vector<Buffer<IGEBufferUsage::index_buffer>*>>& getIndexBuffers();

	void setPolygonMode(VkPolygonMode polygonMode);
	VkPolygonMode polygonMode();

	void setTopology(VkPrimitiveTopology topology);
	VkPrimitiveTopology topology();

	void read(std::string vertex_shader, std::string fragment_shader);
};

