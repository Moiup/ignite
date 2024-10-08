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

void WindowSurface::setInstance(Instance* instance) {
	_instance = instance;
}

const VkSurfaceKHR& WindowSurface::getSurface() const {
	return _surface;
}

std::vector<VkSurfaceFormatKHR> WindowSurface::getSurfaceFormats(const PhysicalDevice& gpu){
	uint32_t surface_format_count{};
	VkResult vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(
		gpu.getGPU(),
		getSurface(),
		&surface_format_count,
		nullptr
	);
	if(vk_result != VK_SUCCESS){
		throw std::runtime_error("Error: failed retreiving physical device surface formats count.");
	}
	std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
	vk_result = vkGetPhysicalDeviceSurfaceFormatsKHR(
		gpu.getGPU(),
		getSurface(),
		&surface_format_count,
		surface_formats.data()
	);
	if(vk_result != VK_SUCCESS){
		throw std::runtime_error("Error: failed retreiving physical device surface formats.");
	}

	return surface_formats;
}

void WindowSurface::create() {
	//VkResult vk_result = glfwCreateWindowSurface(
	//	*_instance,
	//	(GLFWwindow *)getWindow(),
	//	nullptr,
	//	&_surface
	//);
	//if (vk_result != VK_SUCCESS) {
	//	throw std::runtime_error("WindowSurface::create: failed creating surface.");
	//}

	SDL_bool  sdl_result = SDL_Vulkan_CreateSurface(
		getWindow(),
		_instance->getInstance(),
		&_surface
	);
	if (!sdl_result) {
		throw std::runtime_error("Error: failed creating surface.");
	}
}

void WindowSurface::destroy() {
	vkDestroySurfaceKHR(_instance->getInstance(), _surface, nullptr);
}