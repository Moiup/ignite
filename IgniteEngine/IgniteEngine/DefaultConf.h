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
	static DefaultGPU* gpu;
	static LogicalDevice* logical_device;
	static GraphicShader* graphic_shader;
	static CommandBuffer* copy_cmd_buf;
	static WindowSurface* render_window;
	static Instance* instance;
	static Renderer* renderer;
	static CameraScene* camera;
	static Texture* white_texture;
	static uint64_t delta_time; // ms
	static SDL_Event* event;

	static VertexBuffer* coord_buffer;
	static VertexBuffer* uv_buffer;
	static IndexBuffer* index_buffer;
};
