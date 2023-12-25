#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_vulkan.h"

#include "Module.h"
#include "WindowUI.h"
#include "RayTracerCPU.h"

class MenuModule : public Module
{
private:
	WindowUI _menu_window;
	SDL_Renderer* _renderer;

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

