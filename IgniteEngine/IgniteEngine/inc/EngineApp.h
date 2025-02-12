#pragma once

#include "App.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Window.h"
#include "WindowSurface.h"

#include "DefaultRenderer.h"
#include "Modules.h"

#include "DefaultConf.h"

#include <SDL.h>
#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"

class EngineApp: public App
{
private:
	Instance _instance;
	PhysicalDevice _gpu;
	Device _device;

	Queue _g_queue;
	Queue _c_queue;
	Queue _p_queue;

public:
	EngineApp();

	void launch();

	void init();
	void start();
	void update();
	void close();

private:
	void initEngineEntities();
	void startEngineEntities();
	void updateEngineEntities();
	void closeEngineEntities();
};

