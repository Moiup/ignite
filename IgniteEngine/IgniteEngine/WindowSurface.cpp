#include "WindowSurface.h"

WindowSurface::WindowSurface() : Window::Window(), created{false} {
	;
}

WindowSurface::WindowSurface(std::string name) : 
	Window::Window(name), created{ false } {
	;
}

WindowSurface::WindowSurface(uint32_t width, uint32_t height) :
	Window::Window(width, height), created{ false } {
	;
}
WindowSurface::WindowSurface(std::string name, uint32_t width, uint32_t height) :
	Window::Window(name, width, height), created{ false }{
	;
}

void WindowSurface::init() {
	Window::init();
	create();
}

void WindowSurface::close() {
	Window::close();
	destroy();
}

void WindowSurface::setInstance(VkInstance* instance) {
	_instance = instance;
}

const VkSurfaceKHR& WindowSurface::getSurface() const {
	return _surface;
}

void WindowSurface::create() {
	/*VkResult vk_result = glfwCreateWindowSurface(
		*_instance,
		(GLFWwindow *)getWindow(),
		nullptr,
		&_surface
	);*/
	SDL_bool  sdl_result = SDL_Vulkan_CreateSurface(
		getWindow(),
		*_instance,
		&_surface
	);

	if (!sdl_result) {
		throw std::runtime_error("Error: failed creating surface.");
	}
}

void WindowSurface::destroy() {
	vkDestroySurfaceKHR(*_instance, _surface, nullptr);
}