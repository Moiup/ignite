#include "GreenScreen.h"

void GreenScreen::init() {

}

void GreenScreen::start() {
	Pixels img("../../assets/textures/la_boeuf_green.png");

	//img.saveFile("../../assets/textures/la_boeuf_green2.png");

	_input.setQueue(DefaultConf::compute_queue);
	_input.setDimensions(img.getWidth(), img.getHeight());
	_input.setFormat(VK_FORMAT_R8G8B8A8_UINT);
	_input.create();
	_input.update(img);
	_input.changeLayout(VK_IMAGE_LAYOUT_GENERAL);

	_output.setQueue(DefaultConf::compute_queue);
	_output.setDimensions(img.getWidth(), img.getHeight());
	_output.setFormat(VK_FORMAT_R8G8B8A8_UINT);
	_output.create();
	_output.changeLayout(VK_IMAGE_LAYOUT_GENERAL);

	DefaultConf::compute_queue->flush();

	_push_constant._width = img.getWidth();
	_push_constant._height = img.getHeight();
	_push_constant._color = glm::vec4(0, 255, 0, 255);
	_push_constant._hardness = 0.5;
	std::cout << sizeof(_push_constant) << std::endl;

	createShader();

	_compute_pipeline.setShader(&_green_screen_comp);
	_compute_pipeline.create();

	_dispatcher.setQueue(DefaultConf::compute_queue);
	_dispatcher.setComputePipeline(&_compute_pipeline);
	_dispatcher.create();

	//glm::vec4 original_color = img.getPixel(580, 420);
	//original_color = glm::normalize(original_color);
	//std::cout << makeString(original_color) << std::endl;
	//glm::vec4 color = glm::normalize(_push_constant._color);
	//glm::vec4 diff_vec = original_color - color;
	//std::cout << makeString(diff_vec) << std::endl;
	//float dist = glm::(diff_vec);

}

void GreenScreen::update() {
	ImGui::Begin("Green Screen");

	ImGui::SliderFloat(
		"Hardness",
		&_push_constant._hardness,
		0,
		1.0F
		);
	ImGui::ColorPicker4(
		"Color",
		&_push_constant._color.x
	);

	ImGui::End();

	_push_constant._color = glm::vec4(0, 255, 0, 255);

	if (DefaultConf::event->type == SDL_KEYDOWN) {
		if (DefaultConf::event->key.keysym.sym == SDLK_a) {
			std::cout << "oui" << std::endl;
			_dispatcher.dispatch(
				((_input.getWidth() - 1) / 16) + 1,
				((_input.getHeight() - 1) / 16) + 1,
				1);
			_dispatcher.submit();
			_dispatcher.wait();
			Pixels res;
			res.setPixels(_output.getWidth(), _output.getHeight());
			_output.flushPixels(res);
			res.saveFile("../../assets/textures/la_boeuf_green2.png");
		}

	}


}

void GreenScreen::close(){

}

void GreenScreen::createShader() {
	_green_screen_comp.setLogicalDevice(DefaultConf::logical_device);
	_green_screen_comp.read("../../shaders/green_screen/green_screen.comp");

	_green_screen_comp.addStorageTextureInfo(
		"original",
		0,
		VK_SHADER_STAGE_COMPUTE_BIT
	);

	_green_screen_comp.addStorageTextureInfo(
		"res",
		1,
		VK_SHADER_STAGE_COMPUTE_BIT
	);

	_green_screen_comp.addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_COMPUTE_BIT,
		0,
		sizeof(_push_constant)
	);

	_green_screen_comp.addTexture("original", &_input);
	_green_screen_comp.addTexture("res", &_output);
	_green_screen_comp.addPushConstant("pc", &_push_constant);
}
