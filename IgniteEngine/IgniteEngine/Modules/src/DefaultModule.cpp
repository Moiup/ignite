#include "DefaultModule.h"


void DefaultModule::init() {
	Module::init();

	// Setting window
	//const VkInstance& insta = _instance.getInstance();
	//_render_window.setInstance(const_cast<VkInstance*>(&insta));
	_render_window.setInstance(DefaultConf::instance);
	_render_window.setFlags(SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
	_render_window.setWidth(1024);
	_render_window.setHeight(768);
	
	DefaultConf::render_window = &_render_window;
	DefaultConf::graphic_shader = &_graphic_shader;

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

	DefaultConf::coord_buffer = &_coord_buffer;
	DefaultConf::index_buffer = &_index_buffer;
	DefaultConf::uv_buffer = &_uv_buffer;


	// White Texture
	Pixels pixels("../../assets/textures/white.png");
	_white_texture.setQueue(DefaultConf::graphics_queue);
	_white_texture.setDimensions(pixels.getWidth(), pixels.getHeight());
	_white_texture.create();
	_white_texture.update(pixels);
	_white_texture.changeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();
}

void DefaultModule::start() {
	Module::start();

	//-----------//
	// Shader    //
	//-----------//
	DefaultConf::graphic_shader->setNbFrame(DefaultConf::NB_FRAME);
	DefaultConf::graphic_shader->setLogicalDevice(DefaultConf::logical_device);
	DefaultConf::graphic_shader->read(
		"../../shaders/vert.vert",
		"../../shaders/frag.frag"
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
	_obj_tr_buffer.setValues(Object3D::getTransformMatrices(DefaultConf::renderer, DefaultConf::graphic_shader).data());
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
	DefaultConf::renderer->setGraphicsQueues(&DefaultConf::logical_device->getQueues("graphics_queues"));
	DefaultConf::renderer->setPresentQueues(&DefaultConf::logical_device->getQueues("present_queues"));
	DefaultConf::renderer->setNbFrame(DefaultConf::NB_FRAME);
	DefaultConf::renderer->setPhysicalDevice(DefaultConf::gpu);
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


	DefaultConf::renderer->render();
}

void DefaultModule::close() {
	Module::close();


}