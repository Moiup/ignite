#include "ReadWriteImageMod.h"

ReadWriteImageMod::ReadWriteImageMod() : Module::Module() { ; }

void ReadWriteImageMod::init() {
	Module::init();
}

void ReadWriteImageMod::start() {
	Module::start();
	std::cout << "read-write start" << std::endl;

	// Loading the plan
	std::vector<Texture*> textures{ &_dst_texture };
	_plan_info.loadWavefont("../assets/3d_objects/plan_textured/plan_tex.obj");
	_plan.createFromObjectInfo(_plan_info);
	_plan.setRenderer(DefaultConf::renderer);
	_plan.addShader(DefaultConf::graphic_shader);
	_plan.setTexture(textures);

	// Setting the textures
	Pixels src_pixels("../assets/textures/scarecrow.png");
	_src_texture.setQueue(DefaultConf::compute_queue);
	_src_texture.setDimensions(src_pixels.getWidth(), src_pixels.getHeight());
	_src_texture.setFormat(VK_FORMAT_R8G8B8A8_UNORM);
	_src_texture.create();
	_src_texture.update(src_pixels);
	_src_texture.changeLayout(VK_IMAGE_LAYOUT_GENERAL);

	Pixels dst_pixels{};
	dst_pixels.setPixels(src_pixels.getWidth(), src_pixels.getHeight());
	_dst_texture.setQueue(DefaultConf::compute_queue);
	_dst_texture.setDimensions(dst_pixels.getWidth(), dst_pixels.getHeight());
	_dst_texture.setFormat(VK_FORMAT_R8G8B8A8_UNORM);
	_dst_texture.create();
	_dst_texture.update(dst_pixels);
	_dst_texture.changeLayout(VK_IMAGE_LAYOUT_GENERAL);
	DefaultConf::compute_queue->submit();
	DefaultConf::compute_queue->wait();

	// Setting the shader up
	_comp_read_write_shader.setLogicalDevice(DefaultConf::logical_device);
	_comp_read_write_shader.read("../shaders/img_blue.comp");

	_comp_read_write_shader.addStorageTextureInfo(
		"src_image",
		0,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);
	_comp_read_write_shader.addTexture(
		"src_image",
		&_src_texture
	);

	_comp_read_write_shader.addStorageTextureInfo(
		"dst_image",
		1,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);
	_comp_read_write_shader.addTexture(
		"dst_image",
		&_dst_texture
	);

	
	_crws_pc.width = src_pixels.getWidth();
	_crws_pc.height = src_pixels.getHeight();
	_crws_pc.color_sum = 0;

	_comp_read_write_shader.addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_COMPUTE_BIT,
		0,
		sizeof(_crws_pc)
	);
	_comp_read_write_shader.addPushConstant("pc", &_crws_pc);

	// The compute pipeline
	_compute_pipeline.setShader(&_comp_read_write_shader);
	_compute_pipeline.create();

	// Dispatcher
	_dispatcher.setQueue(&DefaultConf::logical_device->getQueues("compute_queues")[0]);
	_dispatcher.setComputePipeline(&_compute_pipeline);
	_dispatcher.create();
}

void ReadWriteImageMod::update() {
	Module::update();

	//std::cout << "Dispatch" << std::endl;
	_dispatcher.dispatch(_crws_pc.width/16, _crws_pc.height/16, 1);

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	_dispatcher.submitAndWait();
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	float delta_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() * 0.001;

	std::cout << "compute time: " << delta_time << std::endl;

	if (DefaultConf::event->type == SDL_KEYDOWN) {
		if (DefaultConf::event->key.keysym.sym == SDLK_s && !_is_pressed_s) {
			//_dst_texture.flushToStaging();
			//DefaultConf::compute_queue->submit();
			//DefaultConf::compute_queue->wait();
			Pixels pixels{};
			pixels.setPixels(_dst_texture.getImageExtentWidth(), _dst_texture.getImageExtentHeight());
			_dst_texture.flushPixels(pixels);
			pixels.saveFile("../assets/textures/out_scarecrow.png");
			_is_pressed_s = true;
			//_crws_pc.color_sum += 10;
		}
	}
	if (DefaultConf::event->type == SDL_KEYUP) {
		_is_pressed_s = false;
	}
	
	_crws_pc.color_sum += 1;
}

void ReadWriteImageMod::close() {
	Module::close();

}