#include "Window.h"

std::vector<Window*> Window::windows;

Window::Window() :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	Window::windows.push_back(this);
	setParam(DEFAULT_NAME, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Window::Window(std::string name) :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	Window::windows.push_back(this);
	setParam(name, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Window::Window(uint32_t width, uint32_t height) :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	Window::windows.push_back(this);
	setParam(DEFAULT_NAME, width, height);
}

Window::Window(std::string name, uint32_t width, uint32_t height) : 
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	Window::windows.push_back(this);
	setParam(name, width, height);
}

void Window::setName(std::string name) {
	_name = name;
}

void Window::setWidth(uint32_t width) {
	_width = width;
}

void Window::setHeight(uint32_t height) {
	_height = height;
}

void Window::setParam(std::string name, uint32_t width, uint32_t height) {
	setName(name);
	setWidth(width);
	setHeight(height);
}

SDL_Window* Window::getWindow() const {
	return _window;
}

const std::string& Window::getName() const {
	return _name;
}

const uint32_t Window::getWidth() const {
	return _width;
}

const uint32_t Window::getHeight() const {
	return _height;
}

const uint32_t Window::getWidthInPixel() const {
	int32_t width;
	SDL_GetWindowSize(_window, &width, nullptr);
	return width;
}

const uint32_t Window::getHeightInPixel() const {
	int32_t height;
	SDL_GetWindowSize(_window, nullptr, &height);
	return height;
}

uint32_t Window::getId() const {
	return _w_id;
}

void Window::init() {
	EngineEntity::init();
	create();
}

void Window::start() { 
	EngineEntity::start();
}

void Window::update() { 
	EngineEntity::update();
}

void Window::close() {
	EngineEntity::close();
	
	uint32_t i = 0;
	for (Window* e : Window::windows) {
		if (e == this) {
			Window::windows.erase(
				Window::windows.begin() + i
			);
			break;
		}
		i++;
	}

	destroy();
}

void Window::create() {	
	/*glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	_window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);*/
	_window = SDL_CreateWindow(_name.c_str(), 100, 100, _width, _height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
	if (!_window) {
		throw std::runtime_error("Error: failed creating the glfw window.");
	}
}

void Window::destroy() {
	if (_window == nullptr) {
		return;
	}

	//glfwDestroyWindow(_window);
	SDL_DestroyWindow(_window);
}

void Window::initAll() {
	for (Window* w : Window::windows) {
		w->init();
	}
}

void Window::startAll() {
	for (Window* w : Window::windows) {
		w->start();
	}
}

void Window::updateAll() {
	for (Window* w : Window::windows) {
		w->update();
	}
}

void Window::closeAll() {
	for (Window* w : Window::windows) {
		w->close();
	}
}