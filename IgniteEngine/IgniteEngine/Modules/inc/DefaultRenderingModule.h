#pragma once

#include "imgui.h"
#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"

#include "Module.h"

#include "DefaultRenderer.h"

class DefaultRenderingModule : public Module
{
private:
	//DefaultGPU _gpu{};
	GraphicShader _graphics_shader{};
	GraphicsPipeline _graphics_pipeline{};
	DefaultRenderer _renderer{};

	// Buffers
	StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer;
	StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer;
	StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer;
	StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer;
	StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer;
	//UniformBuffer _camera_buffer;
	StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer;
	StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer;
	Sampler _sampler{};

	bool _is_first{ true };

	PerspectiveCamera _perspective_camera{};
	CameraScene _camera{};
	glm::mat4 _cam_mvp{};


public:
	using Module::Module;

	void init();
	void start();
	void update();
	void close();
};

