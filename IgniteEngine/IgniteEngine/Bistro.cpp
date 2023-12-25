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
	_bistro_info.loadWavefont("../assets/bistro/bistro/exterior.obj");

}

void Bistro::update() {
	Module::update();
}

void Bistro::close() {
	Module::close();
}
