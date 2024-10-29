#include "DebugScene.h"


DebugScene::DebugScene() :
    Module::Module()
{
    ;
}

void DebugScene::init() {
    Module::init();

	DefaultConf::debug_shader = &_debug_shader;
}

void DebugScene::start() {
    Module::start();

	SkeletonDebug::createCrossMesh(_cross_mesh, 1.0);
	SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

	_cross_obj.setMesh(&_cross_mesh);
	_cross_obj.setMaterial(_cross_material, &_cross_material_indices);
	_cross_obj.addShader(&_debug_shader);
	_cross_obj.setRenderer(DefaultConf::renderer);

	createShader();
	_debug_shader.setTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
}

void DebugScene::update() {
	_camera = DefaultConf::camera->getMVPC();


	_obj_tr_buffer.setValues(
		Object3D::updateTransformMatrices(DefaultConf::renderer, &_debug_shader).data()
	);
}

void DebugScene::close() {
    Module::close();


}

void DebugScene::createShader() {
	_debug_shader.setNbFrame(DefaultConf::NB_FRAME);
	_debug_shader.setLogicalDevice(DefaultConf::logical_device);
	_debug_shader.read(
		"../../shaders/debug/debug.vert",
		"../../shaders/debug/debug.frag"
	);
	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_debug_shader.addVertexBufferInfo(
		"coord",
		Object3D::getCoordsStride(DefaultConf::renderer, &_debug_shader),
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	);
	_debug_shader.addVertexBufferInfo(
		"object_id",
		Object3D::getMeshOffsetsStride(DefaultConf::renderer, &_debug_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	_debug_shader.addVertexBufferInfo(
		"material_id",
		Object3D::getMaterialIndicesStride(DefaultConf::renderer, &_debug_shader),
		VK_FORMAT_R32_UINT,
		2
	);

	// Index Buffer
	_debug_shader.addIndexBufferInfo(
		"index",
		Object3D::getIndicesNbElem(DefaultConf::renderer, &_debug_shader)
	);

	// Push constant
	_debug_shader.addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(DefaultConf::camera->getMVPC())
	);

	// Storage Buffers
// transform
	_debug_shader.addStorageBufferInfo(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_debug_shader.addStorageBufferInfo(
		"MaterialsBuffer",
		2,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	//----------------------//
	// Creating the buffers //
	//----------------------//
	// Coord
	// Mesh offsets
	_coord_buffer.setQueue(DefaultConf::graphics_queue);
	_coord_buffer.setSize(Object3D::getCoordsSize(DefaultConf::renderer, &_debug_shader));
	_coord_buffer.create();
	_coord_buffer.setValues(Object3D::getCoords(DefaultConf::renderer, &_debug_shader).data());
	_debug_shader.addVertexBuffer("coord", &_coord_buffer);

	_object_id_buffer.setQueue(DefaultConf::graphics_queue);
	_object_id_buffer.setSize(Object3D::getObjectIdSize(DefaultConf::renderer, &_debug_shader));
	_object_id_buffer.create();
	_object_id_buffer.setValues(Object3D::getObjectId(DefaultConf::renderer, &_debug_shader).data());
	_debug_shader.addVertexBuffer("object_id", &_object_id_buffer);

	_material_indices_buffer.setQueue(DefaultConf::graphics_queue);
	_material_indices_buffer.setSize(Object3D::getMaterialIndicesSize(DefaultConf::renderer, &_debug_shader));
	_material_indices_buffer.create();
	_material_indices_buffer.setValues(Object3D::getMaterialIndices(DefaultConf::renderer, &_debug_shader).data());
	_debug_shader.addVertexBuffer("material_id", &_material_indices_buffer);


	// Index buffer
	_index_buffer.setQueue(DefaultConf::graphics_queue);
	_index_buffer.setSize(Object3D::getIndicesSize(DefaultConf::renderer, &_debug_shader));
	_index_buffer.create();
	_index_buffer.setValues(Object3D::getIndices(DefaultConf::renderer, &_debug_shader).data());
	_debug_shader.addIndexBuffer("index", &_index_buffer);

	// Push Constant
	_camera = DefaultConf::camera->getMVPC();
	_debug_shader.addPushConstant("pc", &_camera[0][0]);


	// Storage Buffers
	// transform
	_obj_tr_buffer.setQueue(DefaultConf::graphics_queue);;
	_obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(DefaultConf::renderer, &_debug_shader));
	_obj_tr_buffer.create();
	_obj_tr_buffer.setValues(Object3D::getTransformMatrices(DefaultConf::renderer, &_debug_shader).data());
	_debug_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);

	// materials
	_materials_buffer.setQueue(DefaultConf::graphics_queue);
	_materials_buffer.setSize(Object3D::getMaterialsSize(DefaultConf::renderer, &_debug_shader));
	_materials_buffer.create();
	_materials_buffer.setValues(Object3D::getMaterials(DefaultConf::renderer, &_debug_shader).data());
	_debug_shader.addStorageBuffer("MaterialsBuffer", &_materials_buffer);
}
