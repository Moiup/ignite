#include "MenuModule.h"

MenuModule::MenuModule() :
	Module::Module(),
	_is_ray_tracer{false},
	_rt_cpu{}
{
	;
}

void MenuModule::init() {
	Module::init();
};

void MenuModule::start() {
	Module::start();

	_menu_window.setName("Menu");
	_menu_window.create();
};

void MenuModule::update() {
	Module::update();

	_menu_window.newFrame();

	menu();

	_menu_window.present();
};

void MenuModule::close() { 
	Module::close();
}

void MenuModule::menu() {	
	ImGui::Begin("Menu");

	if (ImGui::Button("Open Ray Tracer") && !_is_ray_tracer) {
		_is_ray_tracer = true;
	}
	
	if (_is_ray_tracer) {
		_is_ray_tracer = rayTracer();
	}

	ImGui::End();
}

bool MenuModule::rayTracer() {
	bool is_open{ true };
	bool is_finished = _rt_cpu.getIsFinished();

	ImGui::Begin("Ray Tracer");
	if (ImGui::Button("X") && is_finished) {
		is_open = false;
	}

	int nb_cores = _rt_cpu.getNbCores();
	ImGui::SliderInt("Nb Cores", &nb_cores, 1, SDL_GetCPUCount());
	_rt_cpu.setNbCores(nb_cores);

	if (is_finished) {
		if (ImGui::Button("Launch")) {
			_rt_cpu.setIsFinished(false);
		}
	}
	else {
		ImGui::Button("Wait...");
	}

	ImGui::End();

	return is_open;
}