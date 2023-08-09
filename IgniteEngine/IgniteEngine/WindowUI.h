#pragma once
#include "Window.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class WindowUI : public Window
{
private:
	SDL_Renderer* _renderer;

public:
	WindowUI();
	WindowUI(std::string name);
	WindowUI(uint32_t width, uint32_t height);
	WindowUI(std::string name, uint32_t width, uint32_t height);

	void init();
	void start();

	void newFrame();
	void present();

	void create();
	void destroy();

private:
	void setDefaultFlags();

};

