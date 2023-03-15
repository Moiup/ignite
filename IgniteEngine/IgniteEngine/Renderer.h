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
	LogicalDevice* _logical_device;
	CommandPool* _command_pool;
	PhysicalDevice* _gpu;
	WindowSurface* _window;
	VkClearColorValue _clear_color_value;

	std::vector<CommandBuffer> _command_buffers;

	uint32_t _current_frame;

	std::vector<VkPipelineStageFlags> _pipeline_stage_flags;

public:
	Renderer();

	virtual void create();
	virtual void destroy();
	virtual void render();

	void setNbFrame(uint32_t nb_frame);
	void setLogicalDevice(LogicalDevice* logical_device);
	void setCommandPool(CommandPool* command_pool);
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

