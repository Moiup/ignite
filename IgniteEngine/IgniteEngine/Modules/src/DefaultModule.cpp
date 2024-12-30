#include "DefaultModule.h"


void DefaultModule::init() {
	Module::init();

	// Setting window
	//const VkInstance& insta = _instance.getInstance();
	//_render_window.setInstance(const_cast<VkInstance*>(&insta));
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
		DefaultConf::logical_device->getDevice(),
		_render_window.getWidth(),
		_render_window.getHeight(),
		{ DefaultConf::graphics_queue[0].getFamilyIndex() }
	);

	_swapchain = Swapchain(
		*DefaultConf::logical_device->getDevice(),
		*DefaultConf::logical_device->getGPU(),
		{DefaultConf::graphics_queue[0].getFamilyIndex()},
		_render_window,
		DefaultConf::NB_FRAME,
		_render_window.getWidth(),
		_render_window.getHeight()
	);
	for (Image& img : _swapchain.getImages()) {
		DefaultConf::graphics_queue[0].changeLayout(
			img,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);
	}
	DefaultConf::graphics_queue[0].submit();
	DefaultConf::graphics_queue[0].wait();

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
	conf.cull_mode = VK_CULL_MODE_NONE;
	conf.front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	conf.line_width = 1.0;
	conf.blend_constants = { 1.0, 1.0, 1.0, 1.0 };
	conf.color_attachment_format = _swapchain.getSwapchainInfo().imageFormat;
	conf.depth_attachment_format = _depth_buffer.getImageFormat();
	conf.stencil_attachment_format = _depth_buffer.getImageFormat();

	DefaultConf::configuration = conf;

	DefaultConf::graphics_pipeline = &_graphics_pipeline;
	
	_renderer.setSwapchain(_swapchain);
	_renderer.setDepthBuffer(_depth_buffer);
	DefaultConf::renderer = &_renderer;

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
	DefaultConf::coord_buffer = &_coord_buffer;
	DefaultConf::index_buffer = &_index_buffer;
	DefaultConf::uv_buffer = &_uv_buffer;


	// White Texture
	Pixels pixels("../../assets/textures/white.png");
	_white_texture = Texture2D(
		DefaultConf::logical_device->getDevice(),
		pixels.getWidth(),
		pixels.getHeight()
	);

	StagingBuffer<IGEBufferUsage::transfer> sb = StagingBuffer<IGEBufferUsage::transfer>(
		DefaultConf::logical_device->getDevice(),
		pixels.getSize(),
		pixels.getPixels().data()
	);

	DefaultConf::graphics_queue->changeLayout(_white_texture, VK_IMAGE_LAYOUT_GENERAL);
	DefaultConf::graphics_queue->copy(sb, _white_texture);
	DefaultConf::graphics_queue->changeLayout(_white_texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();
}

void DefaultModule::start() {
	Module::start();

	//-----------//
	// Shader    //
	//-----------//
	_graphics_shader = GraphicShader(
		*DefaultConf::logical_device->getDevice(),
		"../../shaders/vert.vert",
		"../../shaders/frag.frag"
	);

	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_graphics_shader.configureVertexBuffer(
		"coord",
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		Object3D::getCoordsStride(*DefaultConf::renderer, *DefaultConf::graphics_pipeline)
	);
	_graphics_shader.configureVertexBuffer(
		"object_id",
		1,
		VK_FORMAT_R32_UINT,
		Object3D::getMeshOffsetsStride(*DefaultConf::renderer, *DefaultConf::graphics_pipeline)
	);
	_graphics_shader.configureVertexBuffer(
		"material_id",
		2,
		VK_FORMAT_R32_UINT,
		Object3D::getMaterialIndicesStride(*DefaultConf::renderer, *DefaultConf::graphics_pipeline)
	);
	_graphics_shader.configureVertexBuffer(
		"uv",
		3,
		VK_FORMAT_R32G32_SFLOAT,
		Object3D::getUVStride(*DefaultConf::renderer, *DefaultConf::graphics_pipeline)
	);
	// Index Buffer
	_graphics_shader.configureIndexBuffer(
		Object3D::getIndicesNbElem(*DefaultConf::renderer, *DefaultConf::graphics_pipeline)
	);

	// Push constant
	_graphics_shader.configurePushConstant(
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(_cam_mvp)
	);

	// Storage Buffers
	// transform
	_graphics_shader.configureStorageBuffer(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_graphics_shader.configureStorageBuffer(
		"MaterialsBuffer",
		2,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_graphics_shader.configureSampler(
		"samp",
		3,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_graphics_shader.configureTexture2D(
		"textures",
		4,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		Object3D::getTextures2D(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).size()
	);


	// Creating the pipeline
	_graphics_pipeline = GraphicsPipeline(
		_graphics_shader,
		DefaultConf::configuration
	);


	//----------------------//
	// Creating the buffers //
	//----------------------//
	// Coord
	// Mesh offsets
	_coord_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getCoordsSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getCoords(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setVertexBuffer("coord", _coord_buffer);

	_object_id_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getObjectIdSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getObjectId(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setVertexBuffer("object_id", _object_id_buffer);

	_material_indices_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialIndicesSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getMaterialIndices(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setVertexBuffer("material_id", _material_indices_buffer);

	_uv_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getUVSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getUV(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setVertexBuffer("uv", _uv_buffer);

	// Index buffersetDevice(DefaultConf::logical_device->getDevice());
	// 
	_index_buffer = StagingBuffer<IGEBufferUsage::index_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getIndicesSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getIndices(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setIndexBuffer(_index_buffer);

	// Push Constant
	_graphics_pipeline.setPushConstants(&_cam_mvp[0][0]);

	// Uniform buffer
	//_camera_buffer.setLogicalDevice(DefaultConf::logical_device);
	//_camera_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
	//_camera_buffer.setSize(sizeof(_camera.getMVP()));
	//_camera_buffer.create();
	//_camera_buffer.setValues(&_camera.getMVP()[0][0]);
	//DefaultConf::graphic_shader->addUniformBuffer("camera", &_camera_buffer);

	// Storage Buffers
	// transform
	_obj_tr_buffer = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getTransformMatricesSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getTransformMatrices(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setStorageBuffer("obj_tr", _obj_tr_buffer);

	// materials
	_materials_buffer = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialsSize(*DefaultConf::renderer, *DefaultConf::graphics_pipeline),
		Object3D::getMaterials(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);
	_graphics_pipeline.setStorageBuffer("MaterialsBuffer", _materials_buffer);

	// Sampler
	_sampler = Sampler(*DefaultConf::logical_device->getDevice());
	_graphics_pipeline.setSamplers("samp", { &_sampler });

	// Textures
	_graphics_pipeline.setTextures2D(
		"textures",
		Object3D::getTextures2D(*DefaultConf::renderer, *DefaultConf::graphics_pipeline)
	);

	_graphics_pipeline.update();

	// Renderer
	DefaultConf::renderer->setGraphicsQueues(&DefaultConf::logical_device->getQueues("graphics_queues"));
	DefaultConf::renderer->setPresentQueues(&DefaultConf::logical_device->getQueues("present_queues"));
	DefaultConf::renderer->setNbFrame(DefaultConf::NB_FRAME);
	DefaultConf::renderer->setDevice(DefaultConf::logical_device->getDevice());
	DefaultConf::renderer->setWindow(DefaultConf::render_window);
	DefaultConf::renderer->setOffset(0, 0);
	DefaultConf::renderer->setExtent(
		DefaultConf::render_window->getWidth(),
		DefaultConf::render_window->getHeight()
	);

	DefaultConf::renderer->create();
}

void DefaultModule::update() {
	Module::update();

	_cam_mvp = DefaultConf::camera->getMVPC();
	//_cam_mvp = glm::mat4(1.0f);

	//Image& img2 = DefaultConf::renderer->getCurrentFrame();
	//Pixels pix2{};
	//pix2.setPixels(
	//	img2.getImageExtentWidth(),
	//	img2.getImageExtentHeight()
	//);

	//if (_is_first) {
	//	_is_first = false;
	//}
	//else {
	//	img2.getQueue()->wait();

	//}
	//img2.flushPixels(pix2);

	//for (uint32_t i = 0; i < 10; i++) {
	//	for (uint32_t j = 0; j < 20; j++) {
	//		pix2.setPixel(10 + i, 195 + j, 255, 0, 0, 255);
	//	}
	//}
	//
	//img2.update(pix2);

	//img2.getQueue()->submit();
	//img2.getQueue()->wait();

#if 0
	_obj_tr_buffer.setValues(
		Object3D::updateTransformMatrices(*DefaultConf::renderer, *DefaultConf::graphics_pipeline).data()
	);

	DefaultConf::renderer->render();
#endif
}

void DefaultModule::close() {
	Module::close();

	_renderer.destroy();
}