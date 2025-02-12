#include "DefaultInit.h"


void DefaultInit::init() {
	Module::init();

	// Setting window
	const std::string name = "Ignite";
	_render_window = WindowSurface(
		name,
		DefaultConf::render_window_width,
		DefaultConf::render_window_height,
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN,
		*DefaultConf::instance
	);

	DefaultConf::render_window = &_render_window;

	_depth_buffer = DepthBuffer(
		DefaultConf::device,
		_render_window.getWidth(),
		_render_window.getHeight(),
		{ DefaultConf::graphics_queue->getFamilyIndex() }
	);

	_swapchain = Swapchain(
		*DefaultConf::device,
		*DefaultConf::gpu,
		{DefaultConf::graphics_queue->getFamilyIndex()},
		_render_window,
		DefaultConf::NB_FRAME,
		_render_window.getWidth(),
		_render_window.getHeight()
	);
	for (Image& img : _swapchain.getImages()) {
		DefaultConf::graphics_queue->changeLayout(
			img,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);
	}
	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();

	GraphicsPipelineConfiguration conf;
	conf.nb_frame = DefaultConf::NB_FRAME;
	conf.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	conf.viewport.x = 0;
	conf.viewport.y = 0;
	conf.viewport.width = _render_window.getWidth();
	conf.viewport.height = _render_window.getHeight();
	conf.viewport.minDepth = 0;
	conf.viewport.maxDepth = 1.0;
	conf.scissor.offset = {0, 0};
	conf.scissor.extent = {_render_window.getWidth(), _render_window.getHeight()};
	conf.polygon_mode = VK_POLYGON_MODE_FILL;
	conf.cull_mode = VK_CULL_MODE_BACK_BIT;
	conf.front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	conf.line_width = 1.0;
	conf.blend_constants = { 1.0, 1.0, 1.0, 1.0 };
	conf.color_attachment_format = _swapchain.getSwapchainInfo().imageFormat;
	conf.depth_attachment_format = _depth_buffer.getImageFormat();
	conf.stencil_attachment_format = _depth_buffer.getImageFormat();

	DefaultConf::configuration = conf;
	
	_perspective_camera.setAspectRatio(
		static_cast<float>(_render_window.getWidth())
		/ 
		static_cast<float>(_render_window.getHeight())
	);
	_perspective_camera.setFOVY(glm::radians(45.0));
	_perspective_camera.setNear(0.1f);
	_perspective_camera.setFar(100.0f);
	_perspective_camera.setEye(0, 0, 0);
	_perspective_camera.setCenter(0, 0, 1);
	_camera.setCamera(&_perspective_camera);

	DefaultConf::camera = &_camera;
	DefaultConf::white_texture = &_white_texture;
	DefaultConf::swapchain = &_swapchain;
	DefaultConf::depth_buffer = &_depth_buffer;

	// White Texture
	Pixels pixels("../assets/textures/white.png");
	_white_texture = Texture2D(
		DefaultConf::device,
		pixels.getWidth(),
		pixels.getHeight()
	);

	StagingBuffer<IGEBufferUsage::transfer> sb = StagingBuffer<IGEBufferUsage::transfer>(
		DefaultConf::device,
		pixels.getSize(),
		pixels.getPixels().data()
	);

	DefaultConf::graphics_queue->changeLayout(_white_texture, VK_IMAGE_LAYOUT_GENERAL);
	DefaultConf::graphics_queue->copy(sb, _white_texture);
	DefaultConf::graphics_queue->changeLayout(_white_texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();
}

void DefaultInit::start() {
	Module::start();
}

void DefaultInit::update() {
	Module::update();
}

void DefaultInit::close() {
	Module::close();
}