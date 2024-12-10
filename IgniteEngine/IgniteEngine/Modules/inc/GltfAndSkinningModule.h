//#pragma once
//
//#include "Module.h"
//#include "DebugScene.h"
//
//class GltfAndSkinningModule : public Module
//{
//private:
//	LoadedObjectInfo _object_info;
//
//	Object3D _object;
//
//	GraphicShader _lbs_shader{};
//
//	// Buffers
//	StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer{};
//	StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer{};
//	StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer{};
//	StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer{};
//	StagingBuffer<IGEBufferUsage::vertex_buffer> _joints_buffer{};
//	StagingBuffer<IGEBufferUsage::vertex_buffer> _weights_buffer{};
//	StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer{};
//	// Uniform buffer
//	StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer{};
//	StagingBuffer<IGEBufferUsage::storage_buffer> _joint_tr_buffer{};
//	StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer{};
//	Sampler _sampler{};
//
//	PerspectiveCamera _perspective_camera{};
//	CameraScene _camera{};
//	glm::mat4 _cam_mvp{};
//
//	SkeletonDebug _hand_skeleton_debug;
//
//	float _angle{ 0 };
//	int32_t _step{ 0 };
//
//public:
//	GltfAndSkinningModule();
//
//	void init();
//	void start();
//	void update();
//	void close();
//
//private:
//	void shaderCreation();
//};