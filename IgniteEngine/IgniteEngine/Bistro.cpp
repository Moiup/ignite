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
	//_bistro_info.loadWavefont("../assets/3d_objects/plan_textured/plan_tex.obj");
	//_bistro_info.loadWavefont("../assets/3d_objects/cube_textured/cube_tex.obj");
	//_bistro_info.loadWavefont("../assets/3d_objects/cornell.obj");
	//_bistro_info.loadWavefont("../assets/Cat/12221_Cat_v1_l3.obj");
	_bistro_info.loadWavefont("../assets/bistro/bistro/exterior.obj");
	_bistro.createFromObjectInfo(_bistro_info);
	_bistro.setRenderer(DefaultConf::renderer);
	_bistro.addShader(DefaultConf::graphic_shader);
}

void Bistro::update() {
	Module::update();

	//if (modify_vertex) {
	//	std::cout << _bistro_info._meshes[0].getCoords()[0][0] << std::endl;

	//	Mesh* m = const_cast<Mesh*>(_bistro.getMesh());
	//	const std::vector<glm::vec3>& c = m->getCoords();
	//	std::vector<glm::vec3>& cc = const_cast<std::vector<glm::vec3>&>(c);
	//	cc[0] = glm::vec3(2.0f, cc[0].y, cc[0].z);

	//	std::cout << _bistro_info._meshes[0].getCoords()[0][0] << std::endl;

	//	//
	//	// IL FAUT METTRE LE RETOUR DU UPDATECOORDS DANS LE BUFFER
	//	//
	//	DefaultConf::coord_buffer->setValues(
	//		&Object3D::updateCoords(DefaultConf::renderer, DefaultConf::graphic_shader)[0]
	//	);

	//	modify_vertex = !modify_vertex;
	//}

}

void Bistro::close() {
	Module::close();

	for (std::vector<Texture>& vec_tex : _bistro_info._textures) {
		for (Texture& tex : vec_tex) {
			tex.destroy();
		}
	}
}
