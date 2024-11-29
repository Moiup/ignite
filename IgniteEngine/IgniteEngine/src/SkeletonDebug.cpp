#include "SkeletonDebug.h"

void SkeletonDebug::setObject3D(Object3D& object) {
	_object = &object;
}

void SkeletonDebug::setSize(float size) {
	_size = size;
}

void SkeletonDebug::setShader(GraphicShader& shader) {
	_shader = &shader;
}

void SkeletonDebug::create() {
	createCrossMesh(_cross_mesh, _size);
	createCrossMaterial(_cross_material, _cross_material_indices);
	
	const Skeleton* skeleton = _object->getSkeleton();

	_cross_objs.resize(skeleton->joints().size() + 1);
	for (int i = 0; i < skeleton->joints().size(); ++i) {
		if (skeleton->joints()[i].id() >= 6) {
			continue;
		}
		_cross_objs[skeleton->joints()[i].id()].setMesh(&_cross_mesh);
		_cross_objs[skeleton->joints()[i].id()].setMaterial(_cross_material, &_cross_material_indices);
		_cross_objs[skeleton->joints()[i].id()].addShader(_shader);
		_cross_objs[skeleton->joints()[i].id()].setRenderer(DefaultConf::renderer);
		_cross_objs[skeleton->joints()[i].id()].setPositionLocale(skeleton->joints()[i].getPositionLocale());
		_cross_objs[skeleton->joints()[i].id()].setRotationLocale(skeleton->joints()[i].getRotationLocale());

		for (int ii = 0; ii < skeleton->joints()[i].getChildren().size(); ++ii) {
			Joint* child = reinterpret_cast<Joint*>(skeleton->joints()[i].getChildren()[ii]);
			_cross_objs[i].addChild(&_cross_objs[child->id()]);
		}
	}
	//Object3D master_obj;
	//_cross_objs.push_back(master_obj);
	_cross_objs[_cross_objs.size() - 1].addChild(&_cross_objs[0]);
	_cross_objs[_cross_objs.size() - 1].setMesh(&_cross_mesh);
	_cross_objs[_cross_objs.size() - 1].setMaterial(_cross_material, &_cross_material_indices);
	_cross_objs[_cross_objs.size() - 1].addShader(DefaultConf::debug_shader);
	_cross_objs[_cross_objs.size() - 1].setRenderer(DefaultConf::renderer);
	_cross_objs[_cross_objs.size() - 1].setPositionLocale(_object->getPositionLocale());
	_cross_objs[_cross_objs.size() - 1].setRotationLocale(_object->getRotationLocale());
	_cross_objs[_cross_objs.size() - 1].setScaleLocale(_object->getScaleLocale());
}

void SkeletonDebug::update() {
	const Skeleton* skeleton = _object->getSkeleton();
	for (int i = 0; i < skeleton->joints().size(); ++i) {
		_cross_objs[i].setPositionLocale(skeleton->joints()[i].getPositionLocale());
		_cross_objs[i].setRotationLocale(skeleton->joints()[i].getRotationLocale());
		_cross_objs[i].alignmentMatrix() = skeleton->joints()[i].alignmentMatrix();
	}
}

void SkeletonDebug::createCrossMesh(Mesh& cross_mesh, float size) {
	std::vector<glm::vec3> coord{
		glm::vec3(0, 0, 0),
		glm::vec3(size, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, size, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, size)
	};

	std::vector<uint32_t> indices{
		0, 1,
		2, 3,
		4, 5
	};

	cross_mesh.setCoords(&coord[0].x, coord.size());
	cross_mesh.setIndices(indices.data(), indices.size());
}

void SkeletonDebug::createCrossMaterial(std::vector<Material>& cross_material, std::vector<uint32_t>& cross_material_indices) {
	Material red;
	red.Kd = glm::vec3(1, 0, 0);

	Material green;
	green.Kd = glm::vec3(0, 1, 0);

	Material blue;
	blue.Kd = glm::vec3(0, 0, 1);

	cross_material.push_back(red);
	cross_material.push_back(green);
	cross_material.push_back(blue);

	cross_material_indices.push_back(0);
	cross_material_indices.push_back(0);
	cross_material_indices.push_back(1);
	cross_material_indices.push_back(1);
	cross_material_indices.push_back(2);
	cross_material_indices.push_back(2);
}