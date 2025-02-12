#pragma once

#include "Buffer.h"
//#include "CameraScene.h"
#include "GraphicShader.h"
#include "WindowSurface.h"
#include "GraphicsPipeline.h"
#include "Renderer.h"
#include "WindowUI.h"

#include <chrono>

class CameraScene;

class DefaultConf
{
public:
	static GraphicsPipelineConfiguration configuration;
	static uint32_t NB_FRAME;
	static PhysicalDevice* gpu;
	static Device* device;
	static GraphicsPipeline* graphics_pipeline;
	static GraphicsPipeline* debug_pipeline;
	static CommandBuffer* copy_cmd_buf;
	static WindowSurface* render_window;
	static uint32_t render_window_width;
	static uint32_t render_window_height;
	static WindowUI* menu_window;
	static Instance* instance;
	static Renderer* renderer;
	static CameraScene* camera;
	static Queue* graphics_queue;
	static Swapchain* swapchain;
	static Texture2D* white_texture;
	static DepthBuffer* depth_buffer;
	static float delta_time; // ms
	static SDL_Event* event;

	static Buffer<IGEBufferUsage::vertex_buffer>* coord_buffer;
	static Buffer<IGEBufferUsage::vertex_buffer>* uv_buffer;
	static Buffer<IGEBufferUsage::index_buffer>* index_buffer;
};
