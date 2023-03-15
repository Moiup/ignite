#pragma once
#include "Window.h"
#include "Instance.h"
#include <SDL_vulkan.h>

class WindowSurface: public Window
{
private:
	bool created;
	VkSurfaceKHR _surface;
	VkInstance* _instance;

public:
	WindowSurface();
	WindowSurface(std::string name);
	WindowSurface(uint32_t width, uint32_t height);
	WindowSurface(std::string name, uint32_t width, uint32_t height);

	void init();
	void close();

	void setInstance(VkInstance* instance);

	const VkSurfaceKHR& getSurface() const;

private:
	void create();
	void destroy();
};

