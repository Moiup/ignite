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
	_white_texture{},
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
		"VK_LAYER_KHRONOS_validation",
	});
	_instance.create();

	// Setting window
	//const VkInstance& insta = _instance.getInstance();
	//_render_window.setInstance(const_cast<VkInstance*>(&insta));
	_render_window.setInstance(&_instance);
	_render_window.setFlags(SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

	// Initialising
	_gpu.configure(_instance);
	
	// Initialising logical device and queues
	_logical_device.configure(&_gpu);
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
	_command_pool.setLogicalDevice(&_logical_device);
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
	DefaultConf::white_texture = &_white_texture;

	DefaultConf::coord_buffer = &_coord_buffer;
	DefaultConf::index_buffer = &_index_buffer;
	DefaultConf::uv_buffer = &_uv_buffer;


	// White Texture
	Pixels pixels("../assets/textures/white.png");
	_white_texture.setLogicalDevice(DefaultConf::logical_device);
	_white_texture.setGPU(DefaultConf::gpu);
	_white_texture.setCommandPool(DefaultConf::command_pool);
	_white_texture.setDimensions(pixels.getWidth(), pixels.getHeight());
	_white_texture.create();
	_white_texture.update(pixels);
	
	initEngineEntities();

	//_modules.init();
}

void EngineApp::start() {
	App::start();

	startEngineEntities();

	//-----------//
	// Shader    //
	//-----------//
	DefaultConf::graphic_shader->setNbFrame(NB_FRAME);
	DefaultConf::graphic_shader->setLogicalDevice(DefaultConf::logical_device);
	DefaultConf::graphic_shader->setPhysicalDevice(DefaultConf::gpu);
	DefaultConf::graphic_shader->read(
		"../shaders/vert.vert",
		"../shaders/frag.frag"
	);
	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	DefaultConf::graphic_shader->addVertexBufferInfo(
		"coord",
		Object3D::getCoordsStride(DefaultConf::renderer, DefaultConf::graphic_shader),
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	);
	DefaultConf::graphic_shader->addVertexBufferInfo(
		"object_id",
		Object3D::getMeshOffsetsStride(DefaultConf::renderer, DefaultConf::graphic_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	DefaultConf::graphic_shader->addVertexBufferInfo(
		"material_id",
		Object3D::getMaterialIndicesStride(DefaultConf::renderer, DefaultConf::graphic_shader),
		VK_FORMAT_R32_UINT,
		2
	);
	DefaultConf::graphic_shader->addVertexBufferInfo(
		"uv",
		Object3D::getUVStride(DefaultConf::renderer, DefaultConf::graphic_shader),
		VK_FORMAT_R32G32_SFLOAT,
		3
	);
	// Index Buffer
	DefaultConf::graphic_shader->addIndexBufferInfo(
		"index",
		Object3D::getIndicesNbElem(DefaultConf::renderer, DefaultConf::graphic_shader)
	);

	// Push constant
	DefaultConf::graphic_shader->addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(DefaultConf::camera->getMVP())
	);

	// Uniform buffer
	//DefaultConf::graphic_shader->addUniformBufferInfo(
	//	"camera",
	//	0,
	//	VK_SHADER_STAGE_VERTEX_BIT
	//);
	
	// Storage Buffers
	// transform
	DefaultConf::graphic_shader->addStorageBufferInfo(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	DefaultConf::graphic_shader->addStorageBufferInfo(
		"MaterialsBuffer",
		2,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	DefaultConf::graphic_shader->addSamplerInfo(
		"samp",
		3,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	DefaultConf::graphic_shader->addTextureInfo(
		"textures",
		4,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		Object3D::getTextures(DefaultConf::renderer, DefaultConf::graphic_shader).size()
	);

	//----------------------//
	// Creating the buffers //
	//----------------------//
	// Coord
	// Mesh offsets
	_coord_buffer.setLogicalDevice(DefaultConf::logical_device);
	_coord_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_coord_buffer.setSize(Object3D::getCoordsSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_coord_buffer.create();
	_coord_buffer.setValues(Object3D::getCoords(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("coord", &_coord_buffer);

	_object_id_buffer.setLogicalDevice(DefaultConf::logical_device);
	_object_id_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_object_id_buffer.setSize(Object3D::getObjectIdSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_object_id_buffer.create();
	_object_id_buffer.setValues(Object3D::getObjectId(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("object_id", &_object_id_buffer);

	_material_indices_buffer.setLogicalDevice(DefaultConf::logical_device);
	_material_indices_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_material_indices_buffer.setSize(Object3D::getMaterialIndicesSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_material_indices_buffer.create();
	_material_indices_buffer.setValues(Object3D::getMaterialIndices(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("material_id", &_material_indices_buffer);

	_uv_buffer.setLogicalDevice(DefaultConf::logical_device);
	_uv_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_uv_buffer.setSize(Object3D::getUVSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_uv_buffer.create();
	_uv_buffer.setValues(Object3D::getUV(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("uv", &_uv_buffer);

	// Index buffer
	_index_buffer.setLogicalDevice(DefaultConf::logical_device);
	_index_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_index_buffer.setSize(Object3D::getIndicesSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_index_buffer.create();
	_index_buffer.setValues(Object3D::getIndices(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addIndexBuffer("index", &_index_buffer);

	// Push Constant
	DefaultConf::graphic_shader->addPushConstant("pc", & _camera);

	// Uniform buffer
	//_camera_buffer.setLogicalDevice(DefaultConf::logical_device);
	//_camera_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	//_camera_buffer.setSize(sizeof(_camera.getMVP()));
	//_camera_buffer.create();
	//_camera_buffer.setValues(&_camera.getMVP()[0][0]);
	//DefaultConf::graphic_shader->addUniformBuffer("camera", &_camera_buffer);

	// Storage Buffers
	// transform
	_obj_tr_buffer.setLogicalDevice(DefaultConf::logical_device);
	_obj_tr_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_obj_tr_buffer.create();
	_obj_tr_buffer.setValues(&Object3D::getTransformMatrices(DefaultConf::renderer, DefaultConf::graphic_shader)[0][0]);
	DefaultConf::graphic_shader->addStorageBuffer("obj_tr", &_obj_tr_buffer);

	// materials
	_materials_buffer.setLogicalDevice(DefaultConf::logical_device);
	_materials_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	_materials_buffer.setSize(Object3D::getMaterialsSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_materials_buffer.create();
	_materials_buffer.setValues(Object3D::getMaterials(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addStorageBuffer("MaterialsBuffer", &_materials_buffer);

	// Sampler
	_sampler.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
	_sampler.create();
	DefaultConf::graphic_shader->addSampler("samp", &_sampler);

	// Textures
	DefaultConf::graphic_shader->addTexture(
		"textures",
		Object3D::getTextures(DefaultConf::renderer, DefaultConf::graphic_shader)
	);

	// Renderer
	DefaultConf::renderer->setNbFrame(NB_FRAME);
	DefaultConf::renderer->setLogicalDevice(DefaultConf::logical_device);
	DefaultConf::renderer->setCommandPool(DefaultConf::command_pool);
	DefaultConf::renderer->setPhysicalDevice(DefaultConf::gpu);
	DefaultConf::renderer->setWindow(DefaultConf::render_window);
	DefaultConf::renderer->create();
}

void EngineApp::update() {
	/*while (!glfwWindowShouldClose(_render_window.getWindow())) {*/
	
	//for (uint32_t i = 0; i < 1; i++) {
	for (;;) {
		std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
		SDL_Event event{};
		if (SDL_PollEvent(&event)) {
			//ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT 
				|| event.window.event == SDL_WINDOWEVENT_CLOSE
				 && event.window.windowID == SDL_GetWindowID(DefaultConf::render_window->getWindow())) {
				break;
			}
		}

		DefaultConf::event = &event;
		EngineEntity::updateAll();
		_obj_tr_buffer.setValues(
			&Object3D::updateTransformMatrices(
				DefaultConf::renderer, DefaultConf::graphic_shader
			)[0][0]
		);

		//_camera_buffer.setValues(
		//	&DefaultConf::camera->getMVP()[0][0]
		//);

		DefaultConf::renderer->render();
		glfwPollEvents();
		

		std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
		uint64_t delta_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() * 0.001;
		DefaultConf::delta_time = delta_time;
	}

	DefaultConf::logical_device->waitIdle();
}

void EngineApp::close() {
	App::close();

	_coord_buffer.destroy();
	_object_id_buffer.destroy();
	_material_indices_buffer.destroy();
	_uv_buffer.destroy();
	_index_buffer.destroy();
	_camera_buffer.destroy();
	_obj_tr_buffer.destroy();
	_materials_buffer.destroy();
	_sampler.destroy();

	DefaultConf::white_texture->destroy();

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