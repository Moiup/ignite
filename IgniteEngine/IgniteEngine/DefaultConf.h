#pragma once

#include "LogicalDevice.h"
#include "DefaultGPU.h"
#include "GraphicShader.h"
#include "WindowSurface.h"
#include "Renderer.h"

class CameraScene;

class DefaultConf
{
public:
	static DefaultGPU *gpu;
	static LogicalDevice *logical_device;
	static GraphicShader *graphic_shader;
	static CommandPool* command_pool;
	static WindowSurface* render_window;
	static Renderer* renderer;
	static CameraScene* camera;
};

