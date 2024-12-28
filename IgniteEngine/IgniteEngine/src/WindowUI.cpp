#include "WindowUI.h"

WindowUI::WindowUI() :
	Window::Window(),
	_renderer{nullptr}
{
	;
}

WindowUI::WindowUI(std::string name) :
	Window::Window(name, DEFAULT_WIDTH, DEFAULT_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI),
	_renderer{ nullptr }
{
	create();
}

WindowUI::WindowUI(uint32_t width, uint32_t height) :
	Window::Window("", width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI),
	_renderer{ nullptr }
{
	create();
}

WindowUI::WindowUI(std::string name, uint32_t width, uint32_t height) :
	Window::Window(name, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI),
	_renderer{nullptr}
{
	create();
}

WindowUI::WindowUI(const WindowUI& w) {
	*this = w;
}

WindowUI& WindowUI::operator=(const WindowUI& w) {
	WindowUI::destroy();
	Window::operator=(w);
	_renderer = w._renderer;

	return *this;
}


void WindowUI::init() {
	;
}

void WindowUI::start() {
}


void WindowUI::create() {
	//Window::create();
	// Initializing SDL
	_renderer = SDL_CreateRenderer(
		getWindow(),
		-1,
		/*SDL_RENDERER_PRESENTVSYNC | */SDL_RENDERER_ACCELERATED
	);

	if (!_renderer) {
		throw std::runtime_error("WindowUI::create: error creating renderer.");
	}

	ImGui_ImplSDL2_InitForSDLRenderer(getWindow(), _renderer);
	ImGui_ImplSDLRenderer2_Init(_renderer);
}

void WindowUI::destroy() {
	if (getNbShared() > 1) {
		return;
	}

	if (!_renderer) {
		return;
	}
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(getWindow());
	SDL_Quit();
}

void WindowUI::clean() {
	destroy();
	Window::clean();
}

void WindowUI::newFrame() {
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void WindowUI::present() {
	ImGui::Render();
	
	ImGuiIO& io = ImGui::GetIO();

	SDL_RenderSetScale(_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(_renderer, (Uint8)(0), (Uint8)(0), (Uint8)(0), (Uint8)(0));
	SDL_RenderClear(_renderer);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), _renderer);
	SDL_RenderPresent(_renderer);
}