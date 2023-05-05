#pragma once
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <SDL.h>

#include "EngineEntity.h"

#include <string>
#include <vector>
#include <iostream>

class Window: public EngineEntity
{
public:
	static std::vector<Window*> windows;

private:
	static uint32_t _last_w_id;
	uint32_t _w_id;

	const uint32_t DEFAULT_WIDTH{1280};
	const uint32_t DEFAULT_HEIGHT{720};
	const std::string DEFAULT_NAME{ "Ignite" };
	uint32_t _width;
	uint32_t _height;

	//GLFWwindow* _window;
	SDL_Window* _window;
	std::string _name;

public:
	Window();
	Window(std::string name);
	Window(uint32_t width, uint32_t height);
	Window(std::string name, uint32_t width, uint32_t height);

	void setName(std::string name);
	void setWidth(uint32_t width);
	void setHeight(uint32_t height);
	void setParam(std::string name, uint32_t width, uint32_t height);

	void init();
	void start();
	void update();
	void close();

	//GLFWwindow* getWindow() const;
	SDL_Window* getWindow() const;
	const std::string& getName() const;
	const uint32_t getWidth() const;
	const uint32_t getHeight() const;
	const uint32_t getWidthInPixel() const;
	const uint32_t getHeightInPixel() const;
	uint32_t getId() const;

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();

protected:
	void create();
	void destroy();
};

