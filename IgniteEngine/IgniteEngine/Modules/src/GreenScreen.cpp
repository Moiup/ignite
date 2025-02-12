//#include "GreenScreen.h"
//
//void GreenScreen::init() {
//	Module::init();
//
//	_img.readFile("../../assets/textures/la_boeuf_green.png");
//
//	_display_window.setName("Green Screen Removal");
//	_display_window.setInstance(DefaultConf::instance);
//	_display_window.setFlags(SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
//	_display_window.setWidth(_img.getWidth());
//	_display_window.setHeight(_img.getHeight());
//}
//
//void GreenScreen::start() {
//	//img.saveFile("../../assets/textures/la_boeuf_green2.png");
//
//	_input.setQueue(DefaultConf::compute_queue);
//	_input.setDimensions(_img.getWidth(), _img.getHeight());
//	_input.setFormat(VK_FORMAT_R8G8B8A8_UINT);
//	_input.create();
//	_input.update(_img);
//	_input.changeLayout(VK_IMAGE_LAYOUT_GENERAL);
//
//	_output.setQueue(DefaultConf::compute_queue);
//	_output.setDimensions(_img.getWidth(), _img.getHeight());
//	_output.setFormat(VK_FORMAT_R8G8B8A8_UINT);
//	_output.create();
//	_output.changeLayout(VK_IMAGE_LAYOUT_GENERAL);
//
//	DefaultConf::compute_queue->submit();
//	DefaultConf::compute_queue->wait();
//
//	_push_constant._width = _img.getWidth();
//	_push_constant._height = _img.getHeight();
//	_push_constant._color = glm::vec4(0, 255, 0, 255);
//	_push_constant._hardness = 0.5;
//	std::cout << sizeof(_push_constant) << std::endl;
//
//	createShader();
//
//	_compute_pipeline.setShader(&_green_screen_comp);
//	_compute_pipeline.create();
//
//	_dispatcher.setQueue(DefaultConf::compute_queue);
//	_dispatcher.setComputePipeline(&_compute_pipeline);
//	_dispatcher.create();
//
//
//	_rdi.setGraphicsQueues(&DefaultConf::logical_device->getQueues("graphics_queues"));
//	_rdi.setPresentQueues(& DefaultConf::logical_device->getQueues("present_queues"));
//	_rdi.setNbFrame(DefaultConf::NB_FRAME);
//	_rdi.setPhysicalDevice(DefaultConf::gpu);
//	_rdi.setWindow(&_display_window);
//	_rdi.setOffset(0, 0);
//	_rdi.setExtent(
//		_output.getWidth(),
//		_output.getHeight()
//	);
//	_rdi.create();
//}
//
//void GreenScreen::update() {
//	ImGui::Begin("Green Screen");
//
//	ImGui::SliderFloat(
//		"Hardness",
//		&_push_constant._hardness,
//		0,
//		1.0F
//		);
//	glm::vec4 picked_color = glm::vec4(glm::vec3(_push_constant._color) / 255.0f, _push_constant._color.a);
//	ImGui::ColorPicker4(
//		"Color",
//		&picked_color.x
//	);
//	_push_constant._color = glm::vec4(
//		glm::vec3(picked_color) * 255.0f,
//		picked_color.a
//		);
//
//	ImGui::End();
//
//	_dispatcher.dispatch(
//		((_input.getWidth() - 1) / 16) + 1,
//		((_input.getHeight() - 1) / 16) + 1,
//		1);
//	_dispatcher.submit();
//	_dispatcher.wait(); // Must get rid of this wait and have a semaphore between the dispatcher and the renderer.
//
//	_rdi.setImage(&_output);
//	_rdi.render();
//
//	//exit(0);
//}
//
//void GreenScreen::close(){
//
//}
//
//void GreenScreen::createShader() {
//	_green_screen_comp.read("../../shaders/green_screen/green_screen.comp");
//
//	_green_screen_comp.addStorageTextureInfo(
//		"original",
//		0,
//		VK_SHADER_STAGE_COMPUTE_BIT
//	);
//
//	_green_screen_comp.addStorageTextureInfo(
//		"res",
//		1,
//		VK_SHADER_STAGE_COMPUTE_BIT
//	);
//
//	_green_screen_comp.addPushConstantInfo(
//		"pc",
//		VK_SHADER_STAGE_COMPUTE_BIT,
//		0,
//		sizeof(_push_constant)
//	);
//
//	_green_screen_comp.addTexture("original", &_input);
//	_green_screen_comp.addTexture("res", &_output);
//	_green_screen_comp.addPushConstant("pc", &_push_constant);
//}
