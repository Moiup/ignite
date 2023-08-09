#include "MenuModule.h"

MenuModule::MenuModule() :
	Module::Module()
{
	;
}

void MenuModule::init() {
	Module::init();
};

void MenuModule::start() {
	Module::start();

	_menu_window.create();
};

void MenuModule::update() {
	Module::update();

	_menu_window.newFrame();

    ImGui::Begin("Hello, world!");             

    ImGui::Text("This is some useful text.");

	if (ImGui::Button("Button")) {
		std::cout << "PlayStation" << std::endl;
	}

    ImGui::End();

	_menu_window.present();
};

void MenuModule::close() { 
	Module::close();
}