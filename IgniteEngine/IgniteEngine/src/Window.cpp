#include "Window.h"

Window::Window() :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	setParam("", DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Window::Window(std::string name) :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	setParam("", DEFAULT_WIDTH, DEFAULT_HEIGHT);
	create();
}

Window::Window(uint32_t width, uint32_t height) :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	setParam("", width, height);
	create();
}

Window::Window(std::string name, uint32_t width, uint32_t height) : 
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	setParam(name, width, height);
	create();
}

Window::Window(
	std::string name,
	uint32_t width,
	uint32_t height,
	uint32_t flags
) :
	EngineEntity::EngineEntity(),
	_window{ nullptr }
{
	setParam(name, width, height);
	setFlags(flags);
	create();
}

Window::Window(const Window& w) {
	*this = w;
}

Window::~Window() {
	destroy();
}

Window& Window::operator=(const Window& w) {
	Window::destroy();
	EngineEntity::operator=(w);

	_w_id = w._w_id;
	_width = w._width;
	_height = w._height;
	_flags = w._flags;
	_window = w._window;
	_name = w._name;

	return *this;
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

void Window::setFlags(uint32_t flags) {
	_flags = flags;
}

//GLFWwindow* Window::getWindow() const {
//	return _window;
//}

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
	//glfwGetWindowSize(_window, &width, nullptr);
	return width;
}

const uint32_t Window::getHeightInPixel() const {
	int32_t height;
	SDL_GetWindowSize(_window, nullptr, &height);
	//glfwGetWindowSize(_window, nullptr, &height);
	return height;
}

uint32_t Window::getId() const {
	return _w_id;
}

void Window::init() {
	EngineEntity::init();
}

void Window::start() { 
	EngineEntity::start();
}

void Window::update() { 
	EngineEntity::update();
}

void Window::close() {
	EngineEntity::close();
}

void Window::create() {	
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//_window = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
	_window = SDL_CreateWindow(_name.c_str(), 100, 100, _width, _height, _flags);
	if (!_window) {
		throw std::runtime_error("Error: failed creating the sdl window.");
	}
}

void Window::destroy() {
	if (getNbShared() > 1) {
		return;
	}

	if (_window == nullptr) {
		return;
	}

	//glfwDestroyWindow(_window);
	SDL_DestroyWindow(_window);
}

void Window::clean() {
	destroy();
	EngineEntity::clean();
}