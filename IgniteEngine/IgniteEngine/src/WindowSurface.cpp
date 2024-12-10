#include "WindowSurface.h"

WindowSurface::WindowSurface() : Window::Window() {

}

WindowSurface::WindowSurface(const std::string& name, uint32_t width, uint32_t height, uint32_t flags, Instance& instance) :
	WindowSurface::WindowSurface()
{
	setName(name);
	setWidth(width);
	setHeight(height);
	setFlags(flags);
	setInstance(&instance);

	create();
}


WindowSurface::WindowSurface(const WindowSurface& ws) {
	*this = ws;
}

WindowSurface::~WindowSurface() {
	close();
}

WindowSurface& WindowSurface::operator=(const WindowSurface& ws) {
	Window::operator=(ws);

	_surface = ws._surface;
	_instance = _instance;

	return *this;
}

void WindowSurface::init() {
	Window::init();
	
}

void WindowSurface::close() {
	Window::close();

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
	Window::create();
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