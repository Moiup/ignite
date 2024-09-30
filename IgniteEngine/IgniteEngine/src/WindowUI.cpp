#include "WindowUI.h"

WindowUI::WindowUI() :
	Window::Window(),
	_renderer{nullptr}
{
	setDefaultFlags();
}

WindowUI::WindowUI(std::string name) :
	Window::Window(name),
	_renderer{ nullptr }
{
	setDefaultFlags();
}

WindowUI::WindowUI(uint32_t width, uint32_t height) :
	Window::Window(width, height),
	_renderer{ nullptr }
{
	setDefaultFlags();
}

WindowUI::WindowUI(std::string name, uint32_t width, uint32_t height) :
	Window::Window(name, width, height),
	_renderer{nullptr}
{
	setDefaultFlags();
}

void WindowUI::init() {
	;
}

void WindowUI::start() {
}


void WindowUI::create() {
	Window::create();
	// Initializing SDL
	_renderer = SDL_CreateRenderer(
		getWindow(),
		-1,
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
	);

	if (!_renderer) {
		throw std::runtime_error("WindowUI::create: error creating renderer.");
	}

	ImGui_ImplSDL2_InitForSDLRenderer(getWindow(), _renderer);
	ImGui_ImplSDLRenderer2_Init(_renderer);
}

void WindowUI::destroy() {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(getWindow());
	SDL_Quit();
}

void WindowUI::setDefaultFlags() {
	setFlags(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(_renderer);
}