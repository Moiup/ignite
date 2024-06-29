#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_vulkan.h"

#include "Module.h"

#include "DefaultRenderer.h"

class DefaultModule : public Module
{
private:
	//DefaultGPU _gpu{};
	//LogicalDevice _logical_device{};
	WindowSurface _render_window{};
	GraphicShader _graphic_shader{};
	DefaultRenderer _renderer{};

	Texture _white_texture{};

	// Buffers
	VertexBuffer _coord_buffer{};
	VertexBuffer _object_id_buffer{};
	VertexBuffer _material_indices_buffer{};
	VertexBuffer _uv_buffer{};
	IndexBuffer _index_buffer{};
	//UniformBuffer _camera_buffer;
	StorageBuffer _obj_tr_buffer{};
	StorageBuffer _materials_buffer{};
	Sampler _sampler{};

	bool _is_first{ true };

	CameraScene _camera{};
	glm::mat4 _cam_mvp{};


public:
	using Module::Module;

	void init();
	void start();
	void update();
	void close();
};

