#include "Bistro.h"

Bistro::Bistro() :
	Module::Module()
{ ; }

void Bistro::init() {
	Module::init();
}

void Bistro::start() {
	Module::start();

	// -- Loading info
	std::cout << "BISTRO" << std::endl;
	_bistro_info.loadWavefont("../assets/bistro/bistro/exterior.obj");
	std::cout << "LOADED" << std::endl;
	std::cout << std::endl;

	_bistro.createFromObjectInfo(_bistro_info);
	_bistro.setRenderer(DefaultConf::renderer);
	_bistro.addShader(DefaultConf::graphic_shader);
}

void Bistro::update() {
	Module::update();
}

void Bistro::close() {
	Module::close();
}
