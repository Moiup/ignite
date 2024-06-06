#include "EngineApp.h"

EngineApp::EngineApp() :
	App::App(),
	_instance{},
	_gpu{},
	_render_window{},
	_logical_device{},
	_graphic_shader{}
	//_renderer{},
	//_white_texture{},
	//_camera{},
	//_modules{}
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
	_logical_device.setGPU(&_gpu);
	_logical_device.configure();
	_logical_device.defineQueue(
		"graphics_queues",
		{VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT},
		1
	);
	_logical_device.defineQueue(
		"present_queues",
		{ VK_QUEUE_GRAPHICS_BIT },
		1
	);
	_logical_device.defineQueue(
		"compute_queues",
		{ VK_QUEUE_COMPUTE_BIT },
		1
	);
	//_logical_device.setGPU(&_gpu);
	_logical_device.create();

	DefaultConf::gpu = &_gpu;
	DefaultConf::logical_device = &_logical_device;
	DefaultConf::render_window = &_render_window;
	DefaultConf::graphics_queue = &_logical_device.getQueues("graphics_queues")[0];
	DefaultConf::present_queue = &_logical_device.getQueues("present_queues")[0];
	DefaultConf::compute_queue = &_logical_device.getQueues("compute_queues")[0];
	DefaultConf::graphic_shader = &_graphic_shader;

	DefaultConf::instance = &_instance;
	DefaultConf::renderer = &_renderer;
	DefaultConf::camera = &_camera;
	DefaultConf::white_texture = &_white_texture;

	DefaultConf::coord_buffer = &_coord_buffer;
	DefaultConf::index_buffer = &_index_buffer;
	DefaultConf::uv_buffer = &_uv_buffer;


	// White Texture
	Pixels pixels("../assets/textures/white.png");
	_white_texture.setQueue(DefaultConf::graphics_queue);
	_white_texture.setDimensions(pixels.getWidth(), pixels.getHeight());
	_white_texture.create();
	_white_texture.update(pixels);
	_white_texture.changeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();

	//_modules.init();
	initEngineEntities();
}

void EngineApp::start() {
	App::start();

	startEngineEntities();

	//-----------//
	// Shader    //
	//-----------//
	DefaultConf::graphic_shader->setNbFrame(NB_FRAME);
	DefaultConf::graphic_shader->setLogicalDevice(DefaultConf::logical_device);
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
		sizeof(_cam_mvp)
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
	_coord_buffer.setQueue(DefaultConf::graphics_queue);
	_coord_buffer.setSize(Object3D::getCoordsSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_coord_buffer.create();
	_coord_buffer.setValues(Object3D::getCoords(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("coord", &_coord_buffer);

	_object_id_buffer.setQueue(DefaultConf::graphics_queue);
	_object_id_buffer.setSize(Object3D::getObjectIdSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_object_id_buffer.create();
	_object_id_buffer.setValues(Object3D::getObjectId(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("object_id", &_object_id_buffer);

	_material_indices_buffer.setQueue(DefaultConf::graphics_queue);
	_material_indices_buffer.setSize(Object3D::getMaterialIndicesSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_material_indices_buffer.create();
	_material_indices_buffer.setValues(Object3D::getMaterialIndices(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("material_id", &_material_indices_buffer);

	_uv_buffer.setQueue(DefaultConf::graphics_queue);
	_uv_buffer.setSize(Object3D::getUVSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_uv_buffer.create();
	_uv_buffer.setValues(Object3D::getUV(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addVertexBuffer("uv", &_uv_buffer);

	// Index buffer
	_index_buffer.setQueue(DefaultConf::graphics_queue);
	_index_buffer.setSize(Object3D::getIndicesSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_index_buffer.create();
	_index_buffer.setValues(Object3D::getIndices(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addIndexBuffer("index", &_index_buffer);


	// Push Constant
	DefaultConf::graphic_shader->addPushConstant("pc", &_cam_mvp[0][0]);

	// Uniform buffer
	//_camera_buffer.setLogicalDevice(DefaultConf::logical_device);
	//_camera_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	//_camera_buffer.setSize(sizeof(_camera.getMVP()));
	//_camera_buffer.create();
	//_camera_buffer.setValues(&_camera.getMVP()[0][0]);
	//DefaultConf::graphic_shader->addUniformBuffer("camera", &_camera_buffer);

	// Storage Buffers
	// transform
	_obj_tr_buffer.setQueue(DefaultConf::graphics_queue);;
	_obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_obj_tr_buffer.create();
	_obj_tr_buffer.setValues(&Object3D::getTransformMatrices(DefaultConf::renderer, DefaultConf::graphic_shader)[0][0]);
	DefaultConf::graphic_shader->addStorageBuffer("obj_tr", &_obj_tr_buffer);

	// materials
	_materials_buffer.setQueue(DefaultConf::graphics_queue);
	_materials_buffer.setSize(Object3D::getMaterialsSize(DefaultConf::renderer, DefaultConf::graphic_shader));
	_materials_buffer.create();
	_materials_buffer.setValues(Object3D::getMaterials(DefaultConf::renderer, DefaultConf::graphic_shader).data());
	DefaultConf::graphic_shader->addStorageBuffer("MaterialsBuffer", &_materials_buffer);

	// Sampler
	_sampler.setDevice(DefaultConf::logical_device->getDevice());
	_sampler.create();
	DefaultConf::graphic_shader->addSampler("samp", &_sampler);

	// Textures
	DefaultConf::graphic_shader->addTexture(
		"textures",
		Object3D::getTextures(DefaultConf::renderer, DefaultConf::graphic_shader)
	);

	// Renderer
	
	DefaultConf::renderer->setGraphicsQueues(&_logical_device.getQueues("graphics_queues"));
	DefaultConf::renderer->setPresentQueues(&_logical_device.getQueues("present_queues"));
	DefaultConf::renderer->setNbFrame(NB_FRAME);
	DefaultConf::renderer->setPhysicalDevice(DefaultConf::gpu);
	DefaultConf::renderer->setWindow(DefaultConf::render_window);
	DefaultConf::renderer->setOffset(200, 0);
	DefaultConf::renderer->setExtent(
		DefaultConf::render_window->getWidth() - 200,
		DefaultConf::render_window->getHeight()
		);
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
		//_obj_tr_buffer.setValues(
		//	&Object3D::updateTransformMatrices(
		//		DefaultConf::renderer, DefaultConf::graphic_shader
		//	)[0][0]
		//);

		//{
		//	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
		//	float delta_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() * 0.001;
		//	DefaultConf::delta_time = delta_time;

		//	std::cout << delta_time << std::endl;
		//}

		_cam_mvp = DefaultConf::camera->getMVP();

		////_camera_buffer.setValues(
		////	&DefaultConf::camera->getMVP()[0][0]
		////);

		DefaultConf::renderer->render();
	//	glfwPollEvents();
	//	

		std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
		float delta_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() * 0.001;
		DefaultConf::delta_time = delta_time;
		//std::cout << delta_time << "\t" << static_cast<uint32_t>(1000 / delta_time) << std::endl;
	}

	DefaultConf::logical_device->waitIdle();
}

void EngineApp::close() {
	App::close();

	//_coord_buffer.destroy();
	//_object_id_buffer.destroy();
	//_material_indices_buffer.destroy();
	//_uv_buffer.destroy();
	//_index_buffer.destroy();
	////_camera_buffer.destroy();
	//_obj_tr_buffer.destroy();
	//_materials_buffer.destroy();
	//_sampler.destroy();

	//DefaultConf::white_texture->destroy();

	//_renderer.destroy();

	//ImGui::DestroyContext();

	////CameraScene::closeAll();
	//Module::closeAll();
	//Window::closeAll();
	//
	//_graphic_shader.destroy();
	//_command_pool.destroy();
	//_logical_device.destroy();

	//_instance.destroy();
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