#pragma once

#include "App.h"

#include "Instance.h"
#include "DefaultGPU.h"
#include "Window.h"
#include "WindowSurface.h"
#include "LogicalDevice.h"
//#include "Swapchain.h"
//#include "Image.h"
//#include "VertexBuffer.h"
//#include "IndexBuffer.h"
//#include "UniformBuffer.h"
//#include "StorageBuffer.h"
//#include "GraphicsPipeline.h"
#include "DefaultRenderer.h"
//#include "Mesh.h"
//#include "Object3D.h"
#include "Modules.h"
//#include "PerspectiveCamera.h"
//#include "CameraScene.h"
//
#include "DefaultConf.h"

#include <SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

class EngineApp: public App
{
private:
	Instance _instance;
	DefaultGPU _gpu;
	LogicalDevice _logical_device;
	//WindowSurface _render_window;
	//GraphicShader _graphic_shader;
	//DefaultRenderer _renderer;

	//Texture _white_texture;

	//// Buffers
	//VertexBuffer _coord_buffer;
	//VertexBuffer _object_id_buffer;
	//VertexBuffer _material_indices_buffer;
	//VertexBuffer _uv_buffer;
	//IndexBuffer _index_buffer;
	////UniformBuffer _camera_buffer;
	//StorageBuffer _obj_tr_buffer;
	//StorageBuffer _materials_buffer;
	//Sampler _sampler;

	Modules _modules;

	//CameraScene _camera;
	//glm::mat4 _cam_mvp;

public:
	EngineApp();

	void init();
	void start();
	void update();
	void close();

private:
	void initEngineEntities();
	void startEngineEntities();
	void updateEngineEntities();
	void closeEngineEntities();
};

