#include "Hand2D.h"

Hand2D::Hand2D() {
	;
}

void Hand2D::init() {
	Module::init();

}

void rectangle(Mesh& mesh) {
	std::vector<glm::vec3> coords;
	std::vector<uint32_t> indices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> n;

	coords.push_back(glm::vec3(-1.0f, -1.0f, 0.0));
	coords.push_back(glm::vec3(1.0f, -1.0f, 0.0));
	coords.push_back(glm::vec3(1.0f, 1.0f, 0.0));
	coords.push_back(glm::vec3(-1.0f, 1.0f, 0.0));

	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));

	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	mesh.setVertex(coords, indices, uv, n);
}

void Hand2D::start() {
	Module::start();

	rectangle(_circle_mesh);
	_circle_obj.setMesh(&_circle_mesh);
	_circle_obj.setRenderer(DefaultConf::renderer);
	_circle_obj.addShader(DefaultConf::graphic_shader);
}

void Hand2D::update() {
	Module::update();


}

void Hand2D::close() {
	Module::close();

}