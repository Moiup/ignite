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
	//_bistro_info.loadWavefont("../assets/Cat/12221_Cat_v1_l3.obj");
	_bistro_info.loadWavefont("../assets/bistro/bistro/exterior.obj");
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
