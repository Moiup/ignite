#include "EngineApp.h"

EngineApp::EngineApp() :
	App::App(),
	_instance{},
	_gpu{},
	_render_window{},
	_logical_device{},
	_command_pool{},
	_graphic_shader{},
	_renderer{},
	_camera{},
	_modules{}
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

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();


	// Instance
	_instance.setExtensionsAndLayers({
		"VK_LAYER_KHRONOS_validation"
	});
	_instance.create();

	// Setting window
	_render_window.setInstance((VkInstance*)&_instance.getInstance());
	_render_window.setFlags(SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

	// Initialising
	_gpu.configure(_instance);
	
	// Initialising logical device and queues
	_logical_device.configure(&_gpu.getGPU());
	_logical_device.setQueue(
		"graphic_queue",
		{VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT}
	);
	_logical_device.setQueue(
		"present_queue",
		{ VK_QUEUE_GRAPHICS_BIT }
	);
	_logical_device.setQueue(
		"compute_queue",
		{ VK_QUEUE_COMPUTE_BIT }
	);
	_logical_device.create();

	// Command Pool
	_command_pool.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_command_pool.setFlags(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	_command_pool.setQueueFamilyIndex(
		_logical_device.getQueue("graphic_queue")->getInfos()->queueFamilyIndex
	);
	_command_pool.create();

	DefaultConf::gpu = &_gpu;
	DefaultConf::logical_device = &_logical_device;
	DefaultConf::render_window = &_render_window;
	DefaultConf::graphic_shader = &_graphic_shader;
	DefaultConf::instance = &_instance;
	DefaultConf::command_pool = &_command_pool;
	DefaultConf::renderer = &_renderer;
	DefaultConf::camera = &_camera;
	
	initEngineEntities();

	//_modules.init();
}

void EngineApp::start() {
	App::start();

	startEngineEntities();

	// Shader
	_graphic_shader.setNbFrame(NB_FRAME);
	_graphic_shader.setLogicalDevice(
		(VkDevice*)_logical_device.getDevice()
	);
	_graphic_shader.setPhysicalDevice(&_gpu);
	_graphic_shader.read(
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
		"object_id",
		Object3D::getMeshOffsetsStride(&_renderer, &_graphic_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	_graphic_shader.addVertexBufferInfo(
		"material_id",
		Object3D::getIndicesToMaterialsStride(&_renderer, &_graphic_shader),
		VK_FORMAT_R32_UINT,
		2
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
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// Creating the buffers
	// Coord

	// Mesh offsets
	_coord_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_coord_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_coord_buffer.setSize(Object3D::getCoordsSize(&_renderer, &_graphic_shader));
	_coord_buffer.create();
	_coord_buffer.setValues(Object3D::getCoords(&_renderer, &_graphic_shader).data());
	_graphic_shader.addVertexBuffer("coord", &_coord_buffer);

	_object_id_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_object_id_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_object_id_buffer.setSize(Object3D::getObjectIdSize(&_renderer, &_graphic_shader));
	_object_id_buffer.create();
	_object_id_buffer.setValues(Object3D::getObjectId(&_renderer, &_graphic_shader).data());
	_graphic_shader.addVertexBuffer("object_id", &_object_id_buffer);

	_indices_to_mat_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_indices_to_mat_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_indices_to_mat_buffer.setSize(Object3D::getIndicesToMaterialSize(&_renderer, &_graphic_shader));
	_indices_to_mat_buffer.create();
	_indices_to_mat_buffer.setValues(Object3D::getIndicesToMaterials(&_renderer, &_graphic_shader).data());
	_graphic_shader.addVertexBuffer("material_id", &_indices_to_mat_buffer);

	// Index buffer
	_index_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_index_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_index_buffer.setSize(Object3D::getIndicesSize(&_renderer, &_graphic_shader));
	_index_buffer.create();
	_index_buffer.setValues(Object3D::getIndices(&_renderer, &_graphic_shader).data());
	_graphic_shader.addIndexBuffer("index", &_index_buffer);

	// Uniform buffer
	//_camera.getPerspectiveCamera().setPositionAbsolute(0, 0, 2.f);
	_camera_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_camera_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_camera_buffer.setSize(sizeof(_camera.getPerspectiveCamera().getMVP()));
	_camera_buffer.create();
	_camera_buffer.setValues(&_camera.getPerspectiveCamera().getMVP()[0][0]);
	_graphic_shader.addUniformBuffer("camera", &_camera_buffer);

	// Storage Buffers
	_obj_tr_buffer.setLogicalDevice((VkDevice*)_logical_device.getDevice());
	_obj_tr_buffer.setMemoryProperties(_gpu.getMemoryProperties());
	_obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(&_renderer, &_graphic_shader));
	_obj_tr_buffer.create();
	_obj_tr_buffer.setValues(&Object3D::getTransformMatrices(&_renderer, &_graphic_shader)[0][0]);
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
		std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

		SDL_Event event{};
		if (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT 
				|| event.window.event == SDL_WINDOWEVENT_CLOSE
				 && event.window.windowID == SDL_GetWindowID(DefaultConf::render_window->getWindow())) {
				break;
			}
		}
		DefaultConf::event = &event;

		EngineEntity::updateAll();

		_obj_tr_buffer.setValues(
			&Object3D::updateTransformMatrices(&_renderer, &_graphic_shader)[0][0]
		);
		_camera_buffer.setValues(
			&DefaultConf::camera->getPerspectiveCamera().getMVP()[0][0]
		);

		_renderer.render();

		glfwPollEvents();
		

		std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
		uint64_t delta_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() * 0.001;
		DefaultConf::delta_time = delta_time;
	}

	_logical_device.waitIdle();
}

void EngineApp::close() {
	App::close();

	_coord_buffer.destroy();
	_object_id_buffer.destroy();
	_indices_to_mat_buffer.destroy();
	_index_buffer.destroy();
	_camera_buffer.destroy();
	_obj_tr_buffer.destroy();
	_renderer.destroy();

	ImGui::DestroyContext();

	//CameraScene::closeAll();
	Module::closeAll();
	Window::closeAll();
	
	_graphic_shader.destroy();
	_command_pool.destroy();
	_logical_device.destroy();


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