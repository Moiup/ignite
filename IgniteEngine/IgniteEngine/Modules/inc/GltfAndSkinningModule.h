#pragma once

#include "Module.h"

class GltfAndSkinningModule : public Module
{
private:
	LoadedObjectInfo _object_info;

	Object3D _object;

	GraphicShader _lbs_shader{};

	// Buffers
	VertexBuffer _coord_buffer{};
	VertexBuffer _object_id_buffer{};
	VertexBuffer _material_indices_buffer{};
	VertexBuffer _uv_buffer{};
	VertexBuffer _joints_buffer{};
	VertexBuffer _weights_buffer{};
	IndexBuffer _index_buffer{};
	// Uniform buffer
	StorageBuffer _obj_tr_buffer{};
	StorageBuffer _joint_tr_buffer{};
	StorageBuffer _materials_buffer{};
	Sampler _sampler{};

	PerspectiveCamera _perspective_camera{};
	CameraScene _camera{};
	glm::mat4 _cam_mvp{};

	float _angle{ 0 };
	int32_t _step{ 0 };

public:
	GltfAndSkinningModule();

	void init();
	void start();
	void update();
	void close();

private:
	void shaderCreation();
};