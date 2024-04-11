#pragma once

#include "WindowSurface.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GraphicShader.h"
#include "GraphicsPipeline.h"

class Renderer
{
protected:
	uint32_t _nb_frame;
	CommandPool* _command_pool;
	PhysicalDevice* _gpu;
	WindowSurface* _window;
	VkClearColorValue _clear_color_value;

	std::vector<CommandBuffer> _command_buffers;

	std::vector<Queue>* _graphics_queues;
	std::vector<Queue>* _present_queues;

	uint32_t _current_frame;

	std::vector<VkPipelineStageFlags> _pipeline_stage_flags;

public:
	Renderer();

	virtual void create();
	virtual void destroy();
	virtual void render();

	void setGraphicsQueues(std::vector<Queue>* graphics_queues);
	void setPresentQueues(std::vector<Queue>* present_queues);

	void setNbFrame(uint32_t nb_frame);
	void setPhysicalDevice(PhysicalDevice* gpu);
	void setWindow(WindowSurface* _window);
	void setClearColorValue(VkClearColorValue clear_color_value);

protected:
	virtual void createFencesAndSemaphores();
	virtual void createSwapchain();
	virtual void createDepthBuffer();
	virtual void createGraphicsPipeline();
	virtual void createCommandBuffers();
};

