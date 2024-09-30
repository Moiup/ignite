#pragma once
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"
// #include "imgui_impl_vulkan.h"

#include "Module.h"
#include "WindowUI.h"
#include "RayTracerCPU.h"

class MenuModule : public Module
{
private:
	WindowUI _menu_window;
	SDL_Renderer* _renderer;

	float _update_fps{ 0 };
	uint32_t _fps;
	std::string _fps_str{};

	bool _is_ray_tracer;
	RayTracerCPU _rt_cpu;

public:
	MenuModule();

	void init();
	void start();
	void update();
	void close();

private:
	void menu();
	bool rayTracer();


};

