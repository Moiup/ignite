#pragma once

#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"

#include "Module.h"

class DefaultInit: public Module
{
private:
	//DefaultGPU _gpu{};
	WindowSurface _render_window{};
	Swapchain _swapchain;
	DepthBuffer _depth_buffer;
	CommandPool _command_pool;

	Texture2D _white_texture{};

	PerspectiveCamera _perspective_camera{};
	CameraScene _camera{};

public:
	using Module::Module;

	void init();
	void start();
	void update();
	void close();
};

