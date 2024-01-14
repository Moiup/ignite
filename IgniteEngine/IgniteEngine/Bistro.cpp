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
	_bistro_info.loadWavefont("../assets/3d_objects/plan_textured/plan_tex.obj");
	//_bistro_info.loadWavefont("../assets/3d_objects/cube_textured/cube_tex.obj");
	//_bistro_info.loadWavefont("../assets/3d_objects/cornell.obj");
	//_bistro_info.loadWavefont("../assets/Cat/12221_Cat_v1_l3.obj");
	//_bistro_info.loadWavefont("../assets/bistro/bistro/exterior.obj");
	_bistro.createFromObjectInfo(_bistro_info);
	_bistro.setRenderer(DefaultConf::renderer);
	_bistro.addShader(DefaultConf::graphic_shader);
}

void Bistro::update() {
	Module::update();
}

void Bistro::close() {
	Module::close();

	for (std::vector<Texture>& vec_tex : _bistro_info._textures) {
		for (Texture& tex : vec_tex) {
			tex.destroy();
		}
	}
}
