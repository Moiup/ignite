#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_vulkan.h"

#include "Module.h"
#include "WindowUI.h"

class MenuModule : public Module
{
private:
	WindowUI _menu_window;
	SDL_Renderer* _renderer;

public:
	MenuModule();

	void init();
	void start();
	void update();
	void close();

};

