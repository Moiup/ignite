#pragma once

#include "Pipeline.h"
#include "DepthBuffer.h"
#include "GraphicShader.h"

class GraphicsPipeline : public Pipeline
{
private:
	uint32_t _nb_frame;
	Swapchain* _swapchain;
	DepthBuffer* _depth_buffer;
	
	VkPolygonMode _polygon_mode;
	VkCullModeFlags _cull_mode;
	VkFrontFace _front_face;
	float _line_width;

	std::vector<VkViewport> _viewport_arr;
	std::vector<VkRect2D> _scissor_arr;

public:
	GraphicsPipeline();

	void setNbFrame(uint32_t nb_frame);
	void setSwapchain(Swapchain* swapchain);
	void setDepthBuffer(DepthBuffer* depth_buffer);

	void setPolygonMode(VkPolygonMode polygon_mode);
	void setCullMode(VkCullModeFlags cull_mode);
	void setFrontFace(VkFrontFace front_face);
	void setLineWidth(float line_width);

	GraphicShader* getShader();
	const std::vector<VkViewport>& getViewport() const;
	const std::vector<VkRect2D>& getScissors() const;

protected:
	void createPipeline();
	void destroyPipeline();
};

