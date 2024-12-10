#pragma once
#include "Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include <SDL_vulkan.h>

class WindowSurface: public Window
{
private:
	VkSurfaceKHR _surface;
	Instance* _instance;
	int32_t* _nb_shared{ nullptr };

public:
	WindowSurface();
	WindowSurface(const std::string& name, uint32_t width, uint32_t height, uint32_t flags, Instance& instance);

	WindowSurface(const WindowSurface& ws);

	~WindowSurface();

	WindowSurface& operator=(const WindowSurface& ws);

	void init();
	void close();

	void setInstance(Instance* instance);

	const VkSurfaceKHR& getSurface() const;
	std::vector<VkSurfaceFormatKHR> getSurfaceFormats(const PhysicalDevice& gpu);

private:
	void create();
	void destroy();
};

