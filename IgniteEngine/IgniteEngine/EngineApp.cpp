#include "EngineApp.h"

EngineApp::EngineApp() :
	App::App(),
	_instance{},
	_gpu{},
	_render_window{},
	_logical_device{},
	_command_pool{},
	_graphic_shader{},
	_renderer{}
{
	;
}

void EngineApp::init() {
	App::init();
	//glfwInit();
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		throw std::runtime_error("SDL could not be initialized!");
	}
	if (SDL_Vulkan_LoadLibrary(nullptr) == -1) {
		throw std::runtime_error("SDL could not load Vulkan library!");
	}

	// Instance
	_instance.setExtensionsAndLayers({
		"VK_LAYER_KHRONOS_validation"
	});
	_instance.create();

	// Setting window
	_render_window.setInstance((VkInstance*)&_instance.getInstance());

	// Initialising gpu
	DefaultConf::gpu = &_gpu;
	_gpu.configure(_instance);
	
	// Initialising logical device and queues
	DefaultConf::logical_device = &_logical_device;
	_logical_device.configure(&_gpu.getGPU());
	_logical_device.setQueue(
		"graphic_queue",
		{VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT}
	);
	_logical_device.setQueue(
		"present_queue",
		{ VK_QUEUE_GRAPHICS_BIT }
	);
	_logical_device.create();

	// Command Pool
	_command_pool.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_command_pool.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	_command_pool.setQueueFamilyIndex(
		_logical_device.getQueue("graphic_queue")->getInfos()->queueFamilyIndex
	);
	_command_pool.create();

	initEngineEntities();

	_modules.setGraphicsShader(&_graphic_shader);
	_modules.setRenderer(&_renderer);
	_modules.setCamera(&_camera);
	_modules.init();
}

void EngineApp::start() {
	App::start();

	_camera.setAspectRatio(
		static_cast<float>(_render_window.getWidth()) /
		static_cast<float>(_render_window.getHeight())
	);
	_camera.setFOVY(glm::radians(45.0f));
	_camera.setNear(0.1f);
	_camera.setFar(100.0f);
	_camera.setEye(glm::vec3(0, 0, 5));

	startEngineEntities();

	// Shader
	DefaultConf::graphic_shader = &_graphic_shader;
	_graphic_shader.setNbFrame(NB_FRAME);
	_graphic_shader.setLogicalDevice(
		(VkDevice*)_logical_device.getDevice()
	);
	_graphic_shader.setPhysicalDevice(&_gpu);
	_graphic_shader.readShaderFiles(
		"../shaders/vert.spv",
		"../shaders/frag.spv"
	);
	// Configuring the Graphic Shader
	_graphic_shader.addVertexBufferInfo(
		"coord",
		Object3D::getCoordsStride(&_renderer, &_graphic_shader),
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	);
	_graphic_shader.addVertexBufferInfo(
		"mesh_offsets",
		Object3D::getMeshOffsetsStride(&_renderer, &_graphic_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	// Index Buffer
	_graphic_shader.addIndexBufferInfo(
		"index",
		Object3D::getIndicesNbElem(&_renderer, &_graphic_shader)
	);

	// Uniform buffer
	_graphic_shader.addUniformBufferInfo(
		"camera",
		0,
		VK_SHADER_STAGE_VERTEX_BIT
	);
	
	// Storage Buffers
	_graphic_shader.addStorageBufferInfo(
		"obj_tr_i",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	_graphic_shader.addStorageBufferInfo(
		"obj_tr",
		2,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// Creating the buffers
	// Coord

	// Mesh offsets
	_mesh_offsets_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_mesh_offsets_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_mesh_offsets_buffer.setSize(Object3D::getMeshOffsetsSize(&_renderer, &_graphic_shader));
	_mesh_offsets_buffer.create();
	_mesh_offsets_buffer.setValues(Object3D::getMeshOffsets(&_renderer, &_graphic_shader).data());
	_graphic_shader.addVertexBuffer("mesh_offsets", &_mesh_offsets_buffer);

	_coord_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_coord_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_coord_buffer.setSize(Object3D::getCoordsSize(&_renderer, &_graphic_shader));
	_coord_buffer.create();
	_coord_buffer.setValues(Object3D::getCoords(&_renderer, &_graphic_shader).data());
	_graphic_shader.addVertexBuffer("coord", &_coord_buffer);

	// Index buffer
	_index_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_index_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_index_buffer.setSize(Object3D::getIndicesSize(&_renderer, &_graphic_shader));
	_index_buffer.create();
	_index_buffer.setValues(Object3D::getIndices(&_renderer, &_graphic_shader).data());
	_graphic_shader.addIndexBuffer("index", &_index_buffer);

	// Uniform buffer
	_camera_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_camera_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_camera_buffer.setSize(sizeof(_camera.getMVP()));
	_camera_buffer.create();
	_camera_buffer.setValues(&_camera.getMVP()[0][0]);
	_graphic_shader.addUniformBuffer("camera", &_camera_buffer);

	// Storage Buffers
	_obj_tr_i_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_obj_tr_i_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_obj_tr_i_buffer.setSize(Object3D::getTransformIndicesSize(&_renderer, &_graphic_shader));
	_obj_tr_i_buffer.create();
	_obj_tr_i_buffer.setValues(Object3D::getTransformIndices(&_renderer, &_graphic_shader).data());
	_graphic_shader.addStorageBuffer("obj_tr_i", &_obj_tr_i_buffer);
	
	_obj_tr_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_obj_tr_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(&_renderer, &_graphic_shader));
	_obj_tr_buffer.create();
	_obj_tr_buffer.setValues(Object3D::getTransformMatrices(&_renderer, &_graphic_shader).data());
	_graphic_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);

	// Renderer
	_renderer.setNbFrame(NB_FRAME);
	_renderer.setLogicalDevice(&_logical_device);
	_renderer.setCommandPool(&_command_pool);
	_renderer.setPhysicalDevice(&_gpu);
	_renderer.setWindow(&_render_window);
	_renderer.create();
}

void EngineApp::update() {
	/*while (!glfwWindowShouldClose(_render_window.getWindow())) {*/
	for (;;) {
		SDL_Event event{};
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}
		updateEngineEntities();

		_obj_tr_buffer.setValues(
			Object3D::updateTransformMatrices(&_renderer, &_graphic_shader).data()
		);

		_renderer.render();

		glfwPollEvents();
	}

	_logical_device.waitIdle();
}

void EngineApp::close() {
	App::close();

	_coord_buffer.destroy();
	_mesh_offsets_buffer.destroy();
	_index_buffer.destroy();
	_camera_buffer.destroy();
	_obj_tr_i_buffer.destroy();
	_obj_tr_buffer.destroy();
	_renderer.destroy();
	_graphic_shader.destroy();
	_command_pool.destroy();
	_logical_device.destroy();

	closeEngineEntities();

	_instance.destroy();
}

void EngineApp::initEngineEntities() {
	for (EngineEntity* e : EngineEntity::engine_entities) {
		e->init();
	}
}

void EngineApp::startEngineEntities() {
	for (EngineEntity* e : EngineEntity::engine_entities) {
		e->start();
	}
}

void EngineApp::updateEngineEntities() {
	for (EngineEntity* e : EngineEntity::engine_entities) {
		e->update();
	}
}

void EngineApp::closeEngineEntities() {
	while (EngineEntity::engine_entities.size()) {
		EngineEntity* e = EngineEntity::engine_entities[0];
		e->close();
	}

	SDL_Quit();
}