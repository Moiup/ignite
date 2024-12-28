#pragma once
#pragma once

#include <vulkan/vulkan.h>
#include <SDL.h>

#include "EngineEntity.h"

#include <string>
#include <vector>
#include <iostream>

class Window: public EngineEntity
{
public:
	static const uint32_t DEFAULT_WIDTH{1280};
	static const uint32_t DEFAULT_HEIGHT{720};

private:
	static uint32_t _last_w_id;
	uint32_t _w_id;

	uint32_t _width{1600};
	uint32_t _height{900};
	uint32_t _flags{ SDL_WINDOW_SHOWN };

	//GLFWwindow* _window;
	SDL_Window* _window{ nullptr };
	std::string _name = "";

public:
	Window();
	Window(std::string name);
	Window(uint32_t width, uint32_t height);
	Window(std::string name, uint32_t width, uint32_t height);
	Window(std::string name, uint32_t width, uint32_t height, uint32_t flags);
	Window(const Window& w);
	~Window();

	Window& operator=(const Window& w);

	void setName(std::string name);
	void setWidth(uint32_t width);
	void setHeight(uint32_t height);
	void setParam(std::string name, uint32_t width, uint32_t height);
	void setFlags(uint32_t flags);

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

protected:
	void create();
	void destroy();
	void clean();
};

