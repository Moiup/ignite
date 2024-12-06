#pragma once

#include "Buffer.h"
#include "Shader.h"
#include <unordered_map>
#include <fstream>
#include <string>

struct VertexInputDescription {
	VkVertexInputBindingDescription binding_desc;
	VkVertexInputAttributeDescription attribute_desc;
};

class GraphicShader : public Shader
{
private:
	std::unordered_map<std::string, VertexInputDescription> _vertex_input_descs;

	IndexBufferInfo _index_buffer_info;

public:
	GraphicShader();
	GraphicShader(Device& device, const std::string& vertex_shader, const std::string& fragment_shader);
	GraphicShader(const GraphicShader& shader);

	GraphicShader& operator=(const GraphicShader& shader);

	void configureVertexBuffer(
		const std::string& name,
		uint32_t location,
		VkFormat format,
		uint32_t stride = 0,
		uint32_t offset = 0
	);
	const VertexInputDescription& getVertexBufferDescription(const std::string& name) const;
	const std::unordered_map<std::string, VertexInputDescription>& getVertexBufferDescription() const;

	void configureIndexBuffer(
		uint32_t nb_elem,
		VkIndexType index_type = VK_INDEX_TYPE_UINT32
	);
	const IndexBufferInfo& getIndexBufferInfo() const;

	void read(std::string vertex_shader, std::string fragment_shader);
};

