#include "EngineApp.h"

EngineApp::EngineApp() :
	App::App()
{
	;
}

void EngineApp::launch() {
	init();
	start();
	update();
	close();
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
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	// Instance
	//Instance::displayExtensionProperties();
	//Instance::displayLayers();
	std::vector<char *> instance_layer = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_KHRONOS_synchronization2"
	};
	_instance = Instance(instance_layer);
	_instance.displayAvailableGPUs();
	
	// Initialising
	_gpu = _instance.getDefaultGPU();
	std::cout << "Selected GPU:" << std::endl;
	_gpu.displayProperties();
	//_gpu.displayDeviceExtensionProperties();
	//_gpu.displayQueueFamilyProperties();

	_device = Device(_gpu);

	{
		std::optional<Queue> q = _device.getQueue(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		if (!q.has_value()) {
			std::cerr << "Failed retreiving graphics queue" << std::endl;
			exit(EXIT_FAILURE);
		}
		_g_queue = std::move(q.value());
	}

	DefaultConf::gpu = &_gpu;
	DefaultConf::device = &_device;
	DefaultConf::graphics_queue = &_g_queue;
	DefaultConf::instance = &_instance;

	initEngineEntities();
}

void EngineApp::start() {
	App::start();

	startEngineEntities();
}

void EngineApp::update() {
	/*while (!glfwWindowShouldClose(_render_window.getWindow())) {*/

	//for (uint32_t i = 0; i < 1; i++) {
	for (;;) {
		std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
		SDL_Event event{};
		if (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT 
				|| event.window.event == SDL_WINDOWEVENT_CLOSE
				 /*&& event.window.windowID == SDL_GetWindowID(DefaultConf::render_window->getWindow())*/) {
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

		//_cam_mvp = DefaultConf::camera->getMVP();

		////_camera_buffer.setValues(
		////	&DefaultConf::camera->getMVP()[0][0]
		////);

		//DefaultConf::renderer->render();
	//	glfwPollEvents();
	//	

		std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
		float delta_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() * 0.001;
		DefaultConf::delta_time = delta_time;
		// std::cout << "total: " << delta_time << " ms" << "\t" << static_cast<uint32_t>(1000 / delta_time) << " fps" << std::endl;
	}

	DefaultConf::device->waitIdle();
}

void EngineApp::close() {
	App::close();

	closeEngineEntities();
}

void EngineApp::initEngineEntities() {
	for(int32_t i = 0; i < EngineEntity::engine_entities.size(); ++i){
		EngineEntity* e = EngineEntity::engine_entities[i];
		e->init();
	}
}

void EngineApp::startEngineEntities() {
	for (int32_t i = 0; i < EngineEntity::engine_entities.size(); ++i) {
		EngineEntity* e = EngineEntity::engine_entities[i];
		e->start();
	}
}

void EngineApp::updateEngineEntities() {
	for (int32_t i = 0; i < EngineEntity::engine_entities.size(); ++i) {
		EngineEntity* e = EngineEntity::engine_entities[i];
		e->update();
	}
}

void EngineApp::closeEngineEntities() {
	for (int32_t i = 0; i < EngineEntity::engine_entities.size(); ++i) {
		EngineEntity* e = EngineEntity::engine_entities[i];
		e->close();
	}

	SDL_Quit();
}