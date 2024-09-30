#pragma once

#include "LogicalDevice.h"
#include "DefaultGPU.h"
//#include "CameraScene.h"
#include "GraphicShader.h"
#include "WindowSurface.h"
#include "Renderer.h"

#include <chrono>

class CameraScene;

class DefaultConf
{
public:
	static uint32_t NB_FRAME;
	static DefaultGPU* gpu;
	static LogicalDevice* logical_device;
	static GraphicShader* graphic_shader;
	static CommandBuffer* copy_cmd_buf;
	static WindowSurface* render_window;
	static Instance* instance;
	static Renderer* renderer;
	static CameraScene* camera;
	static Queue* graphics_queue;
	static Queue* present_queue;
	static Queue* compute_queue;
	static Texture* white_texture;
	static float delta_time; // ms
	static SDL_Event* event;

	static VertexBuffer* coord_buffer;
	static VertexBuffer* uv_buffer;
	static IndexBuffer* index_buffer;
};
