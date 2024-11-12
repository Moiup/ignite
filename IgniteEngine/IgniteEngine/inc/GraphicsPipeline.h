#pragma once

#include "Pipeline.h"
#include "DepthBuffer.h"
#include "GraphicShader.h"

struct GraphicsPipelineConfiguration {
	uint32_t nb_frame{2};
	VkPrimitiveTopology topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
	VkViewport viewport{};
	VkRect2D scissor{};
	VkPolygonMode polygon_mode;
	VkCullModeFlags cull_mode;
	VkFrontFace front_face;
	float line_width;
	glm::vec4 blend_constants{1.0, 1.0, 1.0, 1.0};
	VkFormat color_attachment_format;
	VkFormat depth_attachment_format;
	VkFormat stencil_attachment_format;
};

class GraphicsPipeline : public Pipeline
{
private:
	GraphicsPipelineConfiguration _pipeline_conf;

	std::unordered_map<std::string, VkBuffer> _vertex_buffers;
	VkBuffer _index_buffer;

protected:
	GraphicsPipeline(GraphicShader& shader);

public:
	GraphicsPipeline();
	GraphicsPipeline(
		GraphicShader& shader,
		const GraphicsPipelineConfiguration& config
	);

	void setVertexBuffer(
		const std::string& name,
		const Buffer<IGEBufferUsage::vertex_buffer>& buff
	);

	const std::unordered_map<std::string, VkBuffer>& getVertexBuffers() const;
	const VkBuffer getIndexBuffer() const;

	const VkViewport& getViewport() const;
	const VkRect2D& getScissors() const;

private:
	void createPipeline();
};

