#pragma once
#include "Module.h"
#include "DefaultDispatcher.h"

class DebugScene : public Module
{
private:

	StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer;
	StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer;
	StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer;
	StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer;
	StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer;
	StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer;
	StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer;

	glm::mat4 _camera;

	Mesh _cross_mesh;
	std::vector<Material> _cross_material;
	std::vector<uint32_t> _cross_material_indices;
	Object3D _cross_obj;

	LoadedObjectInfo _ground_obj_info;
	Object3D _ground;

public:
	GraphicShader _debug_shader;

public:
    DebugScene();

    void init();
    void start();
    void update();
    void close();

private:
	void createShader();
	
};

