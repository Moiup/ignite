#include "ReadWriteImageMod.h"

ReadWriteImageMod::ReadWriteImageMod() : Module::Module() { ; }

void ReadWriteImageMod::init() {
	Module::init();
}

void ReadWriteImageMod::start() {
	Module::start();
	std::cout << "read-write start" << std::endl;

	// Setting the textures
	Pixels src_pixels("../assets/textures/scarecrow.png");
	_src_texture.setQueue(DefaultConf::compute_queue);
	_src_texture.setDimensions(src_pixels.getWidth(), src_pixels.getHeight());
	_src_texture.create();
	_src_texture.update(src_pixels);
	_src_texture.changeLayout(VK_IMAGE_LAYOUT_GENERAL);

	Pixels dst_pixels("../assets/textures/scarecrow.png");
	_dst_texture.setQueue(DefaultConf::compute_queue);
	_dst_texture.setDimensions(dst_pixels.getWidth(), dst_pixels.getHeight());
	_dst_texture.create();
	_dst_texture.update(dst_pixels);
	_dst_texture.changeLayout(VK_IMAGE_LAYOUT_GENERAL);


	// Setting the shader up
	_comp_read_write_shader.setLogicalDevice(DefaultConf::logical_device);
	_comp_read_write_shader.read("../shaders/img_blue.comp");

	_comp_read_write_shader.addStorageTextureInfo(
		"src_image",
		0,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);

	_comp_read_write_shader.addStorageTextureInfo(
		"dst_image",
		1,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);

	_comp_read_write_shader.addTexture(
		"src_image",
		&_src_texture
	);

	_comp_read_write_shader.addTexture(
		"dst_image",
		&_dst_texture
	);


	CRWSPushConstant pc{};
	pc.width = dst_pixels.getWidth();
	pc.height = dst_pixels.getHeight();

	_comp_read_write_shader.addPushConstant("pc", &pc);


}

void ReadWriteImageMod::update() {
	Module::update();

}

void ReadWriteImageMod::close() {
	Module::close();

}