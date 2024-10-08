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
	_menu_window.newFrame();
};

void MenuModule::update() {
	Module::update();


	menu();

	_menu_window.present();
	_menu_window.newFrame();
};

void MenuModule::close() { 
	Module::close();
}

void MenuModule::menu() {	
	ImGui::Begin("Menu");

	ImGui::Text("Performances");

	_update_fps += DefaultConf::delta_time;

	if (_update_fps > 250) {
		_fps = static_cast<uint32_t>(1000.0f / DefaultConf::delta_time);
		_update_fps = 0;
	}
	_fps_str = "FPS: " + std::to_string(_fps);
	ImGui::Text(_fps_str.c_str());

	//ImGui::Text(std::to_string(1 / DefaultConf::delta_time).c_str());

	if (ImGui::Button("Open Ray Tracer")) {
		std::cout << "Open" << std::endl;
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
			ImGui::Text("Tracing...");
			_rt_cpu.setIsFinished(false);
			std::cout << "START" << std::endl;
			_rt_cpu.render();
			std::cout << "FINISHED" << std::endl;
		}
	}
	else {
		ImGui::Button("Wait...");
	}

	ImGui::End();

	return is_open;
}