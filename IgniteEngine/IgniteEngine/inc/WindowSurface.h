#pragma once
#include "Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include <SDL_vulkan.h>

class WindowSurface: public Window
{
private:
	bool created;
	VkSurfaceKHR _surface;
	Instance* _instance;

public:
	WindowSurface();
	WindowSurface(std::string name);
	WindowSurface(uint32_t width, uint32_t height);
	WindowSurface(std::string name, uint32_t width, uint32_t height);

	void init();
	void close();

	void setInstance(Instance* instance);

	const VkSurfaceKHR& getSurface() const;
	std::vector<VkSurfaceFormatKHR> getSurfaceFormats(const PhysicalDevice& gpu);

private:
	void create();
	void destroy();
};

