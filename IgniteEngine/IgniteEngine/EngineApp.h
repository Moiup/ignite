#pragma once

#include "App.h"

#include "Instance.h"
#include "DefaultGPU.h"
#include "Window.h"
#include "WindowSurface.h"
#include "LogicalDevice.h"
#include "CommandPool.h"
#include "Swapchain.h"
#include "Image.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "StorageBuffer.h"
#include "GraphicsPipeline.h"
#include "Renderer.h"
#include "DefaultRenderer.h"
#include "Mesh.h"
#include "Object3D.h"
#include "Modules.h"
#include "PerspectiveCamera.h"
#include "CameraScene.h"

#include "DefaultConf.h"

#include <SDL.h>

class EngineApp: public App
{
public:
	static const uint32_t NB_FRAME{ 2 };

private:
	Instance _instance;
	DefaultGPU _gpu;
	WindowSurface _render_window;
	LogicalDevice _logical_device;
	CommandPool _command_pool;
	GraphicShader _graphic_shader;
	DefaultRenderer _renderer;

	// Buffers
	VertexBuffer _coord_buffer;
	VertexBuffer _object_id_buffer;
	IndexBuffer _index_buffer;
	UniformBuffer _camera_buffer;
	StorageBuffer _obj_tr_buffer;

	Modules _modules;

	CameraScene _camera;
	glm::mat4 cam;

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

