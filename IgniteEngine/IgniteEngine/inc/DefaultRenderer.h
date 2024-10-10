#pragma once
#include "CommandBuffer.h"
#include "GraphicShader.h"
#include "Renderer.h"
#include "Object3D.h"

class DefaultRenderer : public Renderer
{
protected:
	Swapchain _swapchain;
	DepthBuffer _depth_buffer;

	std::vector<GraphicsPipeline> _graphics_pipelines;

	std::vector<Queue> _graphics_queues_in_flight;
	std::vector<Queue> _present_queues_in_flight;
	std::vector<VkSemaphore> _sem_render_starts;
	std::vector<VkSemaphore> _sem_render_ends;

public:
	DefaultRenderer();

	void create();
	void destroy();

	void render();

	Image& getCurrentFrame();

protected:
	void createSemaphores();

	void configureQueues();
	void createSwapchain();
	void createDepthBuffer();
	void createGraphicsPipeline();

	void dynamicRenderingPipelineBarrier(CommandBuffer& cmd_buf);
	void dynamicRenderingPipelineBarrierBack(CommandBuffer& cmd_buf);
	void beginRendering(CommandBuffer& cmd_buf);
};

