#pragma once
#include "CommandBuffer.h"
#include "GraphicShader.h"
#include "Renderer.h"
#include "Object3D.h"

class DefaultRenderer : public Renderer
{
private:
	Swapchain _swapchain;
	DepthBuffer _depth_buffer;

	std::vector<GraphicsPipeline> _graphics_pipelines;

	std::vector<VkSemaphore> _sem_render_starts;
	std::vector<VkSemaphore> _sem_render_ends;
	std::vector<VkFence> _fences;

public:
	DefaultRenderer();

	void create();
	void destroy();

	void render();

protected:
	void createFencesAndSemaphores();

	void createSwapchain();
	void createDepthBuffer();
	void createGraphicsPipeline();
	void createCommandBuffers();

	void dynamicRenderingPipelineBarrier();
	void dynamicRenderingPipelineBarrierBack();
	void beginRendering(GraphicsPipeline& graphics_pipeline);
};

