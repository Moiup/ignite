#include "GltfAndSkinningModule.h"

GltfAndSkinningModule::GltfAndSkinningModule() {
	;
}

void GltfAndSkinningModule::init() {
	;
}

void GltfAndSkinningModule::start() {

	//_cylinder_info.loadGLTF("../../assets/3d_objects/gltf/cube/cube.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/CylinderRigged/cylinder.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/RectangleRigged/rectangle.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/PlaneRigged/plane2bones.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/PlaneRigged/plane3bones.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/PlaneRigged/plane3bones_rotated.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/PlaneRigged/plane3fingers.gltf");
	_object_info.loadGLTF("../../assets/3d_objects/gltf/Hand/hand.gltf");
	//_object_info.loadGLTF("../../assets/3d_objects/gltf/PlaneRigged/plane1bone.gltf");
	_object.createFromObjectInfo(_object_info);
	_object.setRenderer(DefaultConf::renderer);
	_object.addShader(&_lbs_shader);
	//_object.addShader(DefaultConf::graphic_shader);

	_object.setScaleLocale(glm::vec3(0.025));

	_lbs_shader.setPolygonMode(VK_POLYGON_MODE_LINE);

	Skeleton* skeleton = &_object_info._skeletons[0];
	{
		Joint* joint = skeleton->skeleton();
		Joint* j = joint;
		std::cout << "Le j: " << j->id() << " " << j->name() << std::endl;
		//j->getChildren()[0]->getChildren()[0]->getChildren()[0]->setRotationLocale(
		//	0.0f,
		//	0.0,
		//	0.1
		//);

		std::cout << std::endl;
		std::cout << *skeleton << std::endl;
	}

	DebugScene::createCrossMesh(_cross_mesh, 1/0.05);
	DebugScene::createCrossMaterial(_cross_material, _cross_material_indices);

	_cross_objs.resize(skeleton->joints().size() + 1);
	for (int i = 0; i < skeleton->joints().size(); ++i) {
		_cross_objs[i].setMesh(&_cross_mesh);
		_cross_objs[i].setMaterial(_cross_material, &_cross_material_indices);
		_cross_objs[i].addShader(DefaultConf::debug_shader);
		_cross_objs[i].setRenderer(DefaultConf::renderer);
		_cross_objs[i].setPositionLocale(skeleton->joints()[i].getPositionLocale());
		_cross_objs[i].setRotationLocale(skeleton->joints()[i].getRotationLocale());
		std::cout << i << "  ";
		for (int ii = 0; ii < skeleton->joints()[i].getChildren().size(); ++ii) {
			Joint* child = reinterpret_cast<Joint*>(skeleton->joints()[i].getChildren()[ii]);
			_cross_objs[i].addChild(&_cross_objs[child->id()]);
			std::cout << child->id() << "  ";
		}
		std::cout << std::endl;
	}
	//Object3D master_obj;
	//_cross_objs.push_back(master_obj);
	_cross_objs[_cross_objs.size() - 1].addChild(&_cross_objs[0]);
	_cross_objs[_cross_objs.size() - 1].setMesh(&_cross_mesh);
	_cross_objs[_cross_objs.size() - 1].setMaterial(_cross_material, &_cross_material_indices);
	_cross_objs[_cross_objs.size() - 1].addShader(DefaultConf::debug_shader);
	_cross_objs[_cross_objs.size() - 1].setRenderer(DefaultConf::renderer);
	_cross_objs[_cross_objs.size() - 1].setPositionLocale(_object.getPositionLocale());
	_cross_objs[_cross_objs.size() - 1].setRotationLocale(_object.getRotationLocale());
	_cross_objs[_cross_objs.size() - 1].setScaleLocale(_object.getScaleLocale());

	//_cross_objs[_cross_objs.size() - 1].setScaleLocale(glm::vec3(0.025));

	shaderCreation();
}

void GltfAndSkinningModule::update() {
	Module::update();
	_cam_mvp = DefaultConf::camera->getMVPC();

	float add_angle = 1 * (DefaultConf::delta_time * 0.001) * (1 - (_step < 0) * 2);
	//std::cout << _step << " " << add_angle << " " << _angle << std::endl;
	_angle += add_angle;

	Skeleton* skeleton = &_object_info._skeletons[0];
	Joint* joint = const_cast<Joint*>(skeleton->skeleton());
	Joint* j = reinterpret_cast<Joint*>(joint);
	j->getChildren()[0]->getChildren()[0]->getChildren()[0]->setRotationLocale(
		-_angle,
		0.0f, 
		0.0
	);

	j->getChildren()[1]->getChildren()[0]->getChildren()[0]->setRotationLocale(
		0.0f,
		0.0,
		_angle
	);

	//j->getChildren()[0]->getChildren()[0]->getChildren()[1]->setRotationLocaleUpdateChildren(
	//	0.0f,
	//	0.0,
	//	_angle
	//);

	glm::mat4 m = j->getTransform();

	_step++;
	const int32_t limit = 300;
	if (_step > limit) {
		_step = -limit;
	} 

	for (int i = 0; i < skeleton->joints().size(); ++i) {
		_cross_objs[i].setPositionLocale(skeleton->joints()[i].getPositionLocale());
		_cross_objs[i].setRotationLocale(skeleton->joints()[i].getRotationLocale());
	}

	_joint_tr_buffer.setValues(Object3D::updateJointsTransform(DefaultConf::renderer, &_lbs_shader).data());
}

void GltfAndSkinningModule::close() {
	;
}

void GltfAndSkinningModule::shaderCreation() {
	//-----------//
	// Shader    //
	//-----------//
	_lbs_shader.setNbFrame(DefaultConf::NB_FRAME);
	_lbs_shader.setLogicalDevice(DefaultConf::logical_device);
	_lbs_shader.read(
		"../../shaders/lbs.vert",
		"../../shaders/lbs.frag"
	);
	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_lbs_shader.addVertexBufferInfo(
		"coord",
		Object3D::getCoordsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	);
	_lbs_shader.addVertexBufferInfo(
		"object_id",
		Object3D::getMeshOffsetsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	_lbs_shader.addVertexBufferInfo(
		"material_id",
		Object3D::getMaterialIndicesStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32_UINT,
		2
	);
	_lbs_shader.addVertexBufferInfo(
		"uv",
		Object3D::getUVStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32_SFLOAT,
		3
	);
	_lbs_shader.addVertexBufferInfo(
		"joints",
		Object3D::getJointsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32B32A32_UINT,
		4
	);
	_lbs_shader.addVertexBufferInfo(
		"weights",
		Object3D::getWeightsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32B32A32_SFLOAT,
		5
	);

	// Index Buffer
	_lbs_shader.addIndexBufferInfo(
		"index",
		Object3D::getIndicesNbElem(DefaultConf::renderer, &_lbs_shader)
	);

	// Push constant
	_lbs_shader.addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(_cam_mvp)
	);

	// Storage Buffers
	// transform
	_lbs_shader.addStorageBufferInfo(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	_lbs_shader.addStorageBufferInfo(
		"joint_tr",
		2,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_lbs_shader.addStorageBufferInfo(
		"MaterialsBuffer",
		3,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_lbs_shader.addSamplerInfo(
		"samp",
		4,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_lbs_shader.addTextureInfo(
		"textures",
		5,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		Object3D::getTextures(DefaultConf::renderer, &_lbs_shader).size()
	);


	//----------------------//
	// Creating the buffers //
	//----------------------//
	_coord_buffer.setQueue(DefaultConf::graphics_queue);
	_coord_buffer.setSize(Object3D::getCoordsSize(DefaultConf::renderer, &_lbs_shader));
	_coord_buffer.create();
	_coord_buffer.setValues(Object3D::getCoords(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addVertexBuffer("coord", &_coord_buffer);

	_object_id_buffer.setQueue(DefaultConf::graphics_queue);
	_object_id_buffer.setSize(Object3D::getObjectIdSize(DefaultConf::renderer, &_lbs_shader));
	_object_id_buffer.create();
	_object_id_buffer.setValues(Object3D::getObjectId(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addVertexBuffer("object_id", &_object_id_buffer);

	_material_indices_buffer.setQueue(DefaultConf::graphics_queue);
	_material_indices_buffer.setSize(Object3D::getMaterialIndicesSize(DefaultConf::renderer, &_lbs_shader));
	_material_indices_buffer.create();
	_material_indices_buffer.setValues(Object3D::getMaterialIndices(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addVertexBuffer("material_id", &_material_indices_buffer);

	_uv_buffer.setQueue(DefaultConf::graphics_queue);
	_uv_buffer.setSize(Object3D::getUVSize(DefaultConf::renderer, &_lbs_shader));
	_uv_buffer.create();
	_uv_buffer.setValues(Object3D::getUV(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addVertexBuffer("uv", &_uv_buffer);

	_joints_buffer.setQueue(DefaultConf::graphics_queue);
	_joints_buffer.setSize(Object3D::getJointsSize(DefaultConf::renderer, &_lbs_shader));
	_joints_buffer.create();
	_joints_buffer.setValues(Object3D::getJoints(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addVertexBuffer("joints", &_joints_buffer);

	_weights_buffer.setQueue(DefaultConf::graphics_queue);
	_weights_buffer.setSize(Object3D::getWeightsSize(DefaultConf::renderer, &_lbs_shader));
	_weights_buffer.create();
	_weights_buffer.setValues(Object3D::getWeights(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addVertexBuffer("weights", &_weights_buffer);

	// Index buffer
	_index_buffer.setQueue(DefaultConf::graphics_queue);
	_index_buffer.setSize(Object3D::getIndicesSize(DefaultConf::renderer, &_lbs_shader));
	_index_buffer.create();
	_index_buffer.setValues(Object3D::getIndices(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addIndexBuffer("index", &_index_buffer);

	// Push Constant
	_lbs_shader.addPushConstant("pc", &_cam_mvp[0][0]);

	// Storage Buffers
	// transform
	_obj_tr_buffer.setQueue(DefaultConf::graphics_queue);;
	_obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(DefaultConf::renderer, &_lbs_shader));
	_obj_tr_buffer.create();
	_obj_tr_buffer.setValues(&Object3D::getTransformMatrices(DefaultConf::renderer, &_lbs_shader)[0][0]);
	_lbs_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);

	// Joint transform
	_joint_tr_buffer.setQueue(DefaultConf::graphics_queue);;
	_joint_tr_buffer.setSize(Object3D::getJointsTransformSize(DefaultConf::renderer, &_lbs_shader));
	_joint_tr_buffer.create();
	_joint_tr_buffer.setValues(&Object3D::getJointsTransform(DefaultConf::renderer, &_lbs_shader)[0][0]);
	_lbs_shader.addStorageBuffer("joint_tr", &_joint_tr_buffer);

	// materials
	_materials_buffer.setQueue(DefaultConf::graphics_queue);
	_materials_buffer.setSize(Object3D::getMaterialsSize(DefaultConf::renderer, &_lbs_shader));
	_materials_buffer.create();
	_materials_buffer.setValues(Object3D::getMaterials(DefaultConf::renderer, &_lbs_shader).data());
	_lbs_shader.addStorageBuffer("MaterialsBuffer", &_materials_buffer);

	// Sampler
	_sampler.setDevice(DefaultConf::logical_device->getDevice());
	_sampler.create();
	_lbs_shader.addSampler("samp", &_sampler);

	// Textures
	_lbs_shader.addTexture(
		"textures",
		Object3D::getTextures(DefaultConf::renderer, &_lbs_shader)
	);
}