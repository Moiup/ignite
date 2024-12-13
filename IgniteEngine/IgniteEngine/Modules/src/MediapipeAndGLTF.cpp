#include "MediapipeAndGLTF.h"

#define IS_NETWORK 1
#define IS_GLTF 0

MediapipeAndGLTF::MediapipeAndGLTF() : Module::Module() {
	;
}

void MediapipeAndGLTF::init() {
	Module::init();

#if IS_NETWORK
	networkInit();
#else
#if !IS_GLTF
	//readMediapipeFile("../../assets/mediapipe_data/mp_hand_straight.txt");
	readMediapipeFile("../../assets/mediapipe_data/mp_hand_straight.txt");
#endif
#endif  
}

void MediapipeAndGLTF::start() {
	Module::start();

	//_hand_obj_info.loadGLTF("../../assets/3d_objects/gltf/Hand/hand_nails.gltf");
	_hand_obj_info.loadGLTF("../../assets/3d_objects/gltf/Hand/hand_origin.gltf");
	_hand.createFromObjectInfo(_hand_obj_info);
	_hand.setRenderer(_fake_renderer);
	_hand.addGraphicsPipeline(_hand_pipeline);

	_hand.setScaleLocale(glm::vec3(SCALE));

	//std::cout << *_hand.getSkeleton() << std::endl;

	_red_sphere_info.loadWavefont("../../assets/3d_objects/spheres/red_sphere.obj");
	_blue_sphere_info.loadWavefont("../../assets/3d_objects/spheres/blue_sphere.obj");


	SkeletonDebug::createCrossMesh(_cross_mesh, 1.0f);
	SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

	_hand_skeleton.setObject3D(_hand);
	_hand_skeleton.setSize(1 / SCALE);
	_hand_skeleton.create();

	_hand_blue_sphere.resize(_hand.getSkeleton()->joints().size());
	for (const Joint& j : _hand.getSkeleton()->joints()) {
		//		Joint& j = _hand.getSkeleton()->joints()[0] {
		for (const Entity3D* child : j.getChildren()) {
			const Joint* cj = static_cast<const Joint*>(child);
			_hand_blue_sphere[j.id()].addChild(&_hand_blue_sphere[cj->id()]);
		}

		_hand_blue_sphere[j.id()].createFromObjectInfo(_blue_sphere_info);
		_hand_blue_sphere[j.id()].setRenderer(*DefaultConf::renderer);
		_hand_blue_sphere[j.id()].addGraphicsPipeline(*DefaultConf::graphics_pipeline);

		_hand_blue_sphere[j.id()].setPositionLocale(j.getPositionLocale());
		_hand_blue_sphere[j.id()].setRotationLocale(j.getRotationLocale());
		_hand_blue_sphere[j.id()].setScaleLocale(j.getScaleLocale());

	}
	_parent_hand_blue_sphere.addChild(&_hand_blue_sphere[0]);
	_parent_hand_blue_sphere.setScaleLocale(_hand.getScaleLocale());


	_mediapipe_red_sphere.resize(mdph::NB_JOINTS_LFS);
	_mediapipe_cross.resize(mdph::NB_JOINTS_LFS);
	for (uint32_t i = 0; i < mdph::NB_JOINTS_LFS; ++i) {
		_mediapipe_red_sphere[i].createFromObjectInfo(_red_sphere_info);
		_mediapipe_red_sphere[i].setRenderer(*DefaultConf::renderer);
		_mediapipe_red_sphere[i].addGraphicsPipeline(*DefaultConf::graphics_pipeline);
		//_mediapipe_red_sphere[i].setScaleLocale(glm::vec3(SCALE));
	}

	_posGlobalesMediapipe.resize(_hand.getSkeleton()->joints().size());


	_recv_frame_stag_buff = StagingBuffer<IGEBufferUsage::transfer>(
		DefaultConf::logical_device->getDevice(),
		_frame_data.size() * sizeof(_frame_data[0])
	);

	//createDebugShader();
	//createLBSShader();
	createHandShader();
	createCompImageSumShader();

	_img_sum_pc.width = DefaultConf::render_window_width;
	_img_sum_pc.height = DefaultConf::render_window_height;

	_sem_rend_start.resize(DefaultConf::NB_FRAME);
	_sem_rend_end.resize(DefaultConf::NB_FRAME);
	_sem_copy_swap_end.resize(DefaultConf::NB_FRAME);
	_sem_comp_sum_end.resize(DefaultConf::NB_FRAME);

	VkSemaphoreCreateInfo sem_info{};
	sem_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	sem_info.pNext = nullptr;
	sem_info.flags = 0;

	for (int32_t i = 0; i < DefaultConf::NB_FRAME; ++i) {
		vkCreateSemaphore(
			DefaultConf::logical_device->getDevice()->getDevice(),
			&sem_info,
			nullptr,
			&_sem_rend_start[i]
		);

		vkCreateSemaphore(
			DefaultConf::logical_device->getDevice()->getDevice(),
			&sem_info,
			nullptr,
			&_sem_rend_end[i]
		);

		vkCreateSemaphore(
			DefaultConf::logical_device->getDevice()->getDevice(),
			&sem_info,
			nullptr,
			&_sem_copy_swap_end[i]
		);

		vkCreateSemaphore(
			DefaultConf::logical_device->getDevice()->getDevice(),
			&sem_info,
			nullptr,
			&_sem_comp_sum_end[i]
		);
	}


#if IS_NETWORK
	_network_thread = std::thread(&MediapipeAndGLTF::networkProcess, this);
#else
#if IS_GLTF
	fromGLTFToLandmarks("../../assets/3d_objects/gltf/Hand/hand_thumb.gltf");

	for (int i = 0; i < 21; ++i) {
		std::cout << "landmarks\t" << i << "\t: " << makeString(_landmarks._landmarks[0][i]) << std::endl;
	}

	landmarksRotationMatrices(_landmarks);
#else
	createWrist(
		_landmarks,
		_lfs
	);

	retargeting(_lfs, *_hand.getSkeleton());

#endif

#endif
}

void MediapipeAndGLTF::update() {
	Module::update();
	_camera = DefaultConf::camera->getMVPC();
	
	menu();

	Swapchain& swapchain = *DefaultConf::swapchain;
	Queue& g_queue = DefaultConf::logical_device->getQueues("graphics_queues")[0];
	Queue& p_queue = DefaultConf::logical_device->getQueues("present_queues")[0];
	Queue& c_queue = DefaultConf::logical_device->getQueues("compute_queues")[0];

	_to_present_img_i = swapchain.acquireNextImage(
		UINT64_MAX,
		_sem_rend_start[_current_queue_i],
		VK_NULL_HANDLE
	);

	_data_mutex.lock();

	VkOffset2D vk_offset2D = { 0, 0 };
	VkExtent2D vk_extent2D = {
		DefaultConf::render_window_width,
		DefaultConf::render_window_height
	};

	g_queue.beginRendering(
		glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
		swapchain,
		*DefaultConf::depth_buffer,
		vk_offset2D,
		vk_extent2D
	);
	g_queue.bindPipeline(_hand_pipeline);
	g_queue.drawIndexed(
		_hand.getMesh().getIndices().size(),
		1,
		0,
		0,
		0
	);
	g_queue.endRendering(swapchain);

	VkPipelineStageFlags pipeline_vert_stage_flag = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	g_queue.submit(
		1,
		&_sem_rend_start[_current_queue_i],
		&pipeline_vert_stage_flag,
		1,
		&_sem_rend_end[_current_queue_i]
	);

	c_queue.copy(_recv_frame_stag_buff, _video_img);

	// Copying the rendered image from the swapchain
	c_queue.copy(
		swapchain.getCurrentImage(),
		_rendered_img
	);

	// Compute buffer here
	c_queue.dispatchBarrier(
		_image_sum_pipeline,
		(_video_img.getWidth() / 16) + 1,
		(_video_img.getHeight() / 16) + 1,
		1
	);

	// Result is in _sum img, copying to swapchain image to display it
	c_queue.copy(
		_sum_img,
		swapchain.getCurrentImage()
	);

	VkPipelineStageFlags pipeline_comp_stage_flag = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	c_queue.submit(
		1,
		&_sem_rend_end[_current_queue_i],
		&pipeline_comp_stage_flag,
		1,
		&_sem_copy_swap_end[_current_queue_i]
	);

	p_queue.present(
		1,
		&_sem_copy_swap_end[_current_queue_i],
		1,
		&swapchain.getSwapchain(),
		&_to_present_img_i
	);
	c_queue.wait();
	g_queue.wait();

	_data_mutex.unlock();

	_current_queue_i = (_current_queue_i + 1) % DefaultConf::NB_FRAME;
}

void MediapipeAndGLTF::close() {
	Module::close();

	_network_thread.join();

	_mediapipe_stream.Close();

}

void MediapipeAndGLTF::networkInit() {
	_socket = EServerSocket(SERVER_ADDRESS, SERVER_PORT);
	_socket.Initialise();

	std::cout << "Connecting to " << SERVER_ADDRESS << " -- " << SERVER_PORT << std::endl;
	_mediapipe_stream = _socket.Connect();
	std::cout << "Connected!" << std::endl;

	mdph::DimMsg dim_msg;
	_mediapipe_stream.Recv(sizeof(dim_msg), reinterpret_cast<char*>(&dim_msg));
	std::cout << "received: " << std::endl;
	std::cout << "    width: " << dim_msg._width << std::endl;
	std::cout << "    height: " << dim_msg._height << std::endl;

	DefaultConf::render_window_width = dim_msg._width;
	DefaultConf::render_window_height = dim_msg._height;

	_frame_data.resize(dim_msg._width * dim_msg._height * _FRAME_NB_CHANNEL);
}

void MediapipeAndGLTF::networkProcess() {
	for (;;) {
		// Asking for new landmarks
		mdph::OkMsg ok_msg;
		ok_msg._is_ok = true;

		int32_t is_sent = _mediapipe_stream.Send(reinterpret_cast<char*>(&ok_msg), sizeof(ok_msg));
		if (is_sent < sizeof(ok_msg)) {
			break;
		}

		// Receiving new landmarks
		mdph::Landmarks landmarks;
		int32_t is_recv = _mediapipe_stream.Recv(sizeof(landmarks), reinterpret_cast<char*>(&landmarks));

		if (is_recv < sizeof(landmarks)) {
			break;
		}

		// Data to big
		// Sometimes fails to be read in one time
		// Therefore, loop to go through all the chunck of data
		uint32_t fd_size = _frame_data.size() * sizeof(_frame_data[0]);
		int32_t total_read = 0;
		do {
			int32_t nb_read = _mediapipe_stream.Recv(
				fd_size - total_read,
				reinterpret_cast<char*>(_frame_data.data() + total_read)
			);

			if (!is_recv) {
				std::cerr << "Problem reading frame data. Connection lost." << std::endl;
				break;
			}
			total_read += nb_read;
		} while (total_read < fd_size);
		if (total_read < fd_size) {
			break;
		}

		createWrist(
			landmarks,
			_lfs
		);
		retargeting(_lfs, *_hand.getSkeleton());

		_data_mutex.lock();
	
		_recv_frame_stag_buff.setValues(_frame_data.data());
		_hand.setPositionLocale(landmarks._landmarks[0][0] * 30.0f);

		// UPDATE THE BUFFERS HERE
		_obj_tr_hand.setValues(
			Object3D::updateTransformMatrices(_fake_renderer, _hand_pipeline).data()
		);

		_joint_tr_hand.setValues(
			Object3D::updateJointsTransform(_fake_renderer, _hand_pipeline).data()
		);

		_data_mutex.unlock();
	}
}

void MediapipeAndGLTF::retargeting(
	const mdph::LandmarksForSkinning& lfs,
	Skeleton& skeleton
) {
	static bool first = true;
	
	// rescale mediapipe data
	std::vector<glm::vec3> posLocales(_hand.getSkeleton()->joints().size());
	for (auto& joint : _hand.getSkeleton()->joints())
	{
		int id = joint.id();
		glm::vec3 posid = lfs._landmarks[0][id];
		if (joint.getParent())
		{
			int parentid = static_cast<Joint*>(joint.getParent())->id();
			glm::vec3 posparentid = lfs._landmarks[0][parentid];
			posLocales[id] = posid - posparentid;
		}
		else {
			posLocales[id] = posid;
		}
	}
	for (auto& joint : _hand.getSkeleton()->joints())
	{
		int id = joint.id();
		glm::vec3 posLocaleGLTF = joint.getPositionLocale();
		if (joint.getParent())
		{
			int parentid = static_cast<Joint*>(joint.getParent())->id();
			glm::vec3 dir = glm::normalize(posLocales[id]);
			_posGlobalesMediapipe[id] = _posGlobalesMediapipe[parentid] + dir * glm::length(posLocaleGLTF) * SCALE;
		}
		else {
			_posGlobalesMediapipe[id] = posLocaleGLTF;
		}
	}
				
	Joint* wrist = skeleton.skeleton();
	
	// Wrist case is apart
	{
		glm::vec3 from = glm::vec4(wrist->getChildren()[3]->getPositionLocale() * SCALE, 0.0f);
		glm::vec3 to = glm::inverse(wrist->getTranslateLocale() * wrist->getRotateLocale()) * glm::vec4(_posGlobalesMediapipe[1],0.0f);

		wrist->alignmentMatrix() = findRotationMatrix(from, to);
	}

#if 1
	for (int32_t parent_i = 0; parent_i < wrist->getChildren().size(); ++parent_i) {
		Joint* cur = static_cast<Joint*>(wrist->getChildren()[parent_i]);
		Joint* next = cur;
		//glm::mat4 tr = wrist->getTransformLocale();  // l2p
		glm::mat4 tr = wrist->getTranslateLocale() * wrist->getRotateLocale() * wrist->alignmentMatrix();

		while (cur->getChildren().size()) {
			next = static_cast<Joint*>(cur->getChildren()[0]);

			//glm::mat4 tr_l2w = tr * cur->getTransformLocale();
			glm::mat4 tr_l2w = tr * cur->getTranslateLocale() * cur->getRotateLocale();

			glm::vec4 from = glm::vec4(next->getPositionLocale() * SCALE, 0.0f);

			glm::vec4 to_point = glm::vec4(_posGlobalesMediapipe[next->id()], 1.0);
			glm::vec4 toparent_point = glm::vec4(_posGlobalesMediapipe[cur->id()], 1.0);
			glm::vec3 to = glm::inverse(tr_l2w) * (to_point-toparent_point);


			static int i = 0;

			i++;

			cur->alignmentMatrix() = findRotationMatrix(from, to);

			tr = tr * cur->getTranslateLocale() * cur->getRotateLocale() * cur->alignmentMatrix();

			cur = next;
		}
	}
#endif
}

void MediapipeAndGLTF::createWrist(
	const mdph::Landmarks& mediapipe_landmarks,
	mdph::LandmarksForSkinning& lfs
) {
	glm::vec3 wrist = mediapipe_landmarks._landmarks[0][0] - 0.2f * (mediapipe_landmarks._landmarks[0][9] - mediapipe_landmarks._landmarks[0][0]);
	lfs._landmarks[0][0] = glm::vec3(0);
	int32_t offset = 0;
	for (int32_t i = 1; i < mdph::NB_JOINTS_LFS; ++i) {
		if (i % 5 == 1) {
			offset++;
			lfs._landmarks[0][i] = mediapipe_landmarks._landmarks[0][0] - wrist;
			//std::cout << i << " -- " << 0 << std::endl;
		}
		else {
			lfs._landmarks[0][i] = mediapipe_landmarks._landmarks[0][i - offset] - wrist;
			//std::cout << i << " -- " << i - offset << std::endl;
		}
		lfs._landmarks[0][i] = lfs._landmarks[0][i];
	}
	//std::cout << std::endl;

}

void MediapipeAndGLTF::readMediapipeFile(const std::string& path) {
	std::ifstream mp_file = std::ifstream(path);

	if (!mp_file.is_open()) {
		std::cerr << "MediapipeAndGLTF::readMediapipeFile: Error opening file '" + path + "'." << std::endl;
		throw std::runtime_error("MediapipeAndGLTF::readMediapipeFile: Error opening file '" + path + "'.");
	}

	// -- Initialisation
	// Reading dimensions
	mp_file >> _mediapipe_info._dim._width >> _mediapipe_info._dim._height;

	DefaultConf::render_window_width = _mediapipe_info._dim._width * 2;
	DefaultConf::render_window_height = _mediapipe_info._dim._height * 2;

	// Reading description
	mp_file >> _mediapipe_info._desc._nb_joints >> _mediapipe_info._desc._nb_hierarchy;

	// Reading joints hierarchy
	_mediapipe_info._hierarchy._hierarchy.resize(_mediapipe_info._desc._nb_joints);
	for (int32_t i = 0; i < _mediapipe_info._desc._nb_hierarchy; ++i) {
		int32_t nb_sons{0};
		int32_t parent_i;
		mp_file >> parent_i >> nb_sons;
		_mediapipe_info._hierarchy._hierarchy[parent_i].resize(nb_sons);
		for (int32_t j = 0; j < nb_sons; ++j) {
			mp_file >> _mediapipe_info._hierarchy._hierarchy[parent_i][j];
		}
	}

	// -- Reading the landmarks --
	int32_t i = 0;
	while (mp_file >> _landmarks._landmarks[0][i].x
		>> _landmarks._landmarks[0][i].y
		>> _landmarks._landmarks[0][i].z
		>> _landmarks._world_landmarks[0][i].x
		>> _landmarks._world_landmarks[0][i].y
		>> _landmarks._world_landmarks[0][i].z
		) {
		++i;
	}

	mp_file.close();
}

void MediapipeAndGLTF::fromGLTFToLandmarks(const std::string& path) {
	_hand_info_to_gltf.loadGLTF(path);

	const Joint* wrist = _hand_info_to_gltf._skeletons[0].skeleton();

	_landmarks._landmarks[0][0] = wrist->getPositionLocale();

	// Wrist -> wrist_bis -> phal1 -> phal2 -> phal3
	Joint* to_modify = static_cast<Joint*>(wrist->getChildren()[1]->getChildren()[0]->getChildren()[0]);
	to_modify->setRotationLocale(
		to_modify->getRotationLocale().x,
		to_modify->getRotationLocale().y,
		to_modify->getRotationLocale().z + glm::radians(-30.0f)
	);

	for (int32_t i = 0; i < 5; ++i) {
		const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[i]);
		const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
		const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
		const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
		const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);

		_landmarks._landmarks[0][i * 4 + 1] = wrist->getTransformLocale() * wrist_bis->getTransformLocale() * glm::vec4(phal1->getPositionLocale(), 1.0);
		_landmarks._landmarks[0][i * 4 + 2] = wrist->getTransformLocale() * wrist_bis->getTransformLocale() * phal1->getTransformLocale() * glm::vec4(phal2->getPositionLocale(), 1.0);
		_landmarks._landmarks[0][i * 4 + 3] = wrist->getTransformLocale() * wrist_bis->getTransformLocale() * phal1->getTransformLocale() * phal2->getTransformLocale() * glm::vec4(phal3->getPositionLocale(), 1.0);
		_landmarks._landmarks[0][i * 4 + 4] = wrist->getTransformLocale() * wrist_bis->getTransformLocale() * phal1->getTransformLocale() * phal2->getTransformLocale() * phal3->getTransformLocale() * glm::vec4(phal4->getPositionLocale(), 1.0);
	}
}

glm::mat4 MediapipeAndGLTF::findRotationMatrix(
	const glm::vec3& from,
	const glm::vec3& to
) {
	glm::mat4 rot = alignVectorMatrix(
		glm::normalize(from),
		glm::normalize(to)
	);
	glm::mat4 res = rot;
	res[3][3] = 1.0f;
	return rot;
}


glm::mat3 MediapipeAndGLTF::alignVectorMatrix(
	const glm::vec3& from,
	const glm::vec3& to
) {
	glm::vec3 refl = glm::vec3(0, 0, 1);
	if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
		refl = glm::vec3(1, 0, 0);
	}
	else if (std::abs(from.y) < 0.72 && std::abs(to.y) < 0.72) {
		refl = glm::vec3(0, 1, 0);
	}

	glm::vec3 u = refl - from;
	glm::vec3 v = refl - to;
	glm::mat3 r = glm::mat3(1);
	for (int32_t i = 0; i < 3; ++i) {
		for (int32_t j = 0; j < 3; ++j) {
			r[i][j] = ((i == j) ? 1 : 0) -
				2 / glm::dot(u, u) * u[i] * u[j] -
				2 / glm::dot(v, v) * v[i] * v[j] +
				4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
		}
	}

	return glm::transpose(r);
}

std::vector<glm::vec3> MediapipeAndGLTF::cube() {
	std::vector<glm::vec3> cube = {
		// Front
		glm::vec3(-1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(-1, 1, -1),
		// Right
		glm::vec3(1, -1, -1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, -1),
		// Back
		glm::vec3(1, -1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(-1, 1, 1),
		glm::vec3(-1, -1, 1),
		// Left
		glm::vec3(-1, -1, 1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, 1, -1),
		glm::vec3(-1, 1, 1),
		// Top
		glm::vec3(-1, 1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(1, 1, 1),
		glm::vec3(-1, 1, 1),
		// Bottom
		glm::vec3(-1, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, -1, -1),
		glm::vec3(-1, -1, -1)
	};
	return cube;
}

std::vector<uint32_t> MediapipeAndGLTF::cubeIndex() {
	std::vector<uint32_t> index{};

	for (uint32_t i = 0; i < 6; i++) {
		index.push_back(0 + i * 4);
		index.push_back(1 + i * 4);
		index.push_back(3 + i * 4);
		index.push_back(1 + i * 4);
		index.push_back(2 + i * 4);
		index.push_back(3 + i * 4);
	}

	return index;
}

void MediapipeAndGLTF::createLBSShader() {
	//-----------//
	// Shader    //
	//-----------//
	_lbs_shader = GraphicShader(
		*DefaultConf::logical_device->getDevice(),
		"../../shaders/lbs.vert",
		"../../shaders/lbs.frag"
	);

	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_lbs_shader.configureVertexBuffer(
		"coord",
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		Object3D::getCoordsStride(*DefaultConf::renderer, _lbs_pipeline)
	);
	_lbs_shader.configureVertexBuffer(
		"object_id",
		1,
		VK_FORMAT_R32_UINT,
		Object3D::getMeshOffsetsStride(*DefaultConf::renderer, _lbs_pipeline)
	);
	_lbs_shader.configureVertexBuffer(
		"material_id",
		2,
		VK_FORMAT_R32_UINT,
		Object3D::getMaterialIndicesStride(*DefaultConf::renderer, _lbs_pipeline)
	);
	_lbs_shader.configureVertexBuffer(
		"uv",
		3,
		VK_FORMAT_R32G32_SFLOAT,
		Object3D::getUVStride(*DefaultConf::renderer, _lbs_pipeline)
	);
	_lbs_shader.configureVertexBuffer(
		"joints",
		4,
		VK_FORMAT_R32G32B32A32_UINT,
		Object3D::getJointsStride(*DefaultConf::renderer, _lbs_pipeline)
	);
	_lbs_shader.configureVertexBuffer(
		"weights",
		5,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		Object3D::getWeightsStride(*DefaultConf::renderer, _lbs_pipeline)
	);

	// Index Buffer
	_lbs_shader.configureIndexBuffer(
		Object3D::getIndicesNbElem(*DefaultConf::renderer, _lbs_pipeline)
	);

	// Push constant
	_lbs_shader.configurePushConstant(
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(_camera)
	);

	// Storage Buffers
	// transform
	_lbs_shader.configureStorageBuffer(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	_lbs_shader.configureStorageBuffer(
		"joint_tr",
		2,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_lbs_shader.configureStorageBuffer(
		"MaterialsBuffer",
		3,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_lbs_shader.configureSampler(
		"samp",
		4,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_lbs_shader.configureTexture2D(
		"textures",
		5,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		Object3D::getTextures2D(*DefaultConf::renderer, _lbs_pipeline).size()
	);

	//----------------------//
	// Creating the buffers //
	//----------------------//
	_coord_buffer_lbs = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getCoordsSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getCoords(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	_object_id_buffer_lbs = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getObjectIdSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getObjectId(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	_material_indices_buffer_lbs = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialIndicesSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getMaterialIndices(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	_uv_buffer_lbs = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getUVSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getUV(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	_joints_buffer_lbs = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getJointsSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getJoints(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	_weights_buffer_lbs = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getWeightsSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getWeights(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	// Index buffer
	_index_buffer_lbs = StagingBuffer<IGEBufferUsage::index_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getIndicesSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getIndices(*DefaultConf::renderer, _lbs_pipeline).data()
	);


	// Storage Buffers
	// transform
	_obj_tr_buffer_lbs = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getTransformMatricesSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getTransformMatrices(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	// Joint transform
	_joint_tr_buffer_lbs = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getJointsTransformSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getJointsTransform(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	// materials
	_materials_buffer_lbs = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialsSize(*DefaultConf::renderer, _lbs_pipeline),
		Object3D::getMaterials(*DefaultConf::renderer, _lbs_pipeline).data()
	);

	// Sampler
	_sampler_hand.setDevice(DefaultConf::logical_device->getDevice());
	_sampler_hand.create();

	
	// --------------------- //
	// Creating the pipeline //
	// --------------------- //
	GraphicsPipelineConfiguration conf = DefaultConf::configuration;
	conf.polygon_mode = VK_POLYGON_MODE_LINE;
	conf.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	conf.line_width = 1.0;

	_lbs_pipeline = GraphicsPipeline(
		_lbs_shader,
		conf
	);

	// Push Constant
	_lbs_pipeline.setPushConstants(&_camera[0][0]);
	_lbs_pipeline.setVertexBuffer("coord", _coord_buffer_lbs);
	_lbs_pipeline.setVertexBuffer("object_id", _object_id_buffer_lbs);
	_lbs_pipeline.setVertexBuffer("material_id", _material_indices_buffer_lbs);
	_lbs_pipeline.setVertexBuffer("uv", _uv_buffer_lbs);
	_lbs_pipeline.setVertexBuffer("joints", _joints_buffer_lbs);
	_lbs_pipeline.setVertexBuffer("weights", _weights_buffer_lbs);
	_lbs_pipeline.setIndexBuffer(_index_buffer_lbs);
	_lbs_pipeline.setStorageBuffer("obj_tr", _obj_tr_buffer_lbs);
	_lbs_pipeline.setStorageBuffer("joint_tr", _joint_tr_buffer_lbs);
	_lbs_pipeline.setStorageBuffer("MaterialsBuffer", _materials_buffer_lbs);
	_lbs_pipeline.setSamplers("samp", { &_sampler_hand });
	// Textures
	_lbs_pipeline.setTextures2D(
		"textures",
		Object3D::getTextures2D(*DefaultConf::renderer, _lbs_pipeline)
	);

	_lbs_pipeline.update();
}

void MediapipeAndGLTF::createDebugShader() {
	_debug_shader = GraphicShader(
		*DefaultConf::logical_device->getDevice(),
		"../../shaders/debug/debug.vert",
		"../../shaders/debug/debug.frag"
	);

	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_debug_shader.configureVertexBuffer(
		"coord",
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		Object3D::getCoordsStride(*DefaultConf::renderer, _debug_pipeline)
	);
	_debug_shader.configureVertexBuffer(
		"object_id",
		1,
		VK_FORMAT_R32_UINT,
		Object3D::getMeshOffsetsStride(*DefaultConf::renderer, _debug_pipeline)
	);
	_debug_shader.configureVertexBuffer(
		"material_id",
		2,
		VK_FORMAT_R32_UINT,
		Object3D::getMaterialIndicesStride(*DefaultConf::renderer, _debug_pipeline)
	);

	// Index Buffer
	_debug_shader.configureIndexBuffer(
		Object3D::getIndicesNbElem(*DefaultConf::renderer, _debug_pipeline)
	);

	// Push constant
	_debug_shader.configurePushConstant(
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(DefaultConf::camera->getMVPC())
	);

	// Storage Buffers
	// transform
	_debug_shader.configureStorageBuffer(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_debug_shader.configureStorageBuffer(
		"MaterialsBuffer",
		2,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	//----------------------//
	// Creating the buffers //
	//----------------------//
	// Coord
	// Mesh offsets
	_coord_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getCoordsSize(*DefaultConf::renderer, _debug_pipeline),
		Object3D::getCoords(*DefaultConf::renderer, _debug_pipeline).data()
	);

	_object_id_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getObjectIdSize(*DefaultConf::renderer, _debug_pipeline),
		Object3D::getObjectId(*DefaultConf::renderer, _debug_pipeline).data()
	);

	_material_indices_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialIndicesSize(*DefaultConf::renderer, _debug_pipeline),
		Object3D::getMaterialIndices(*DefaultConf::renderer, _debug_pipeline).data()
	);

	// Index buffer
	_index_buffer = StagingBuffer<IGEBufferUsage::index_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getIndicesSize(*DefaultConf::renderer, _debug_pipeline),
		Object3D::getIndices(*DefaultConf::renderer, _debug_pipeline).data()
	);

	// Storage Buffers
	// transform
	_obj_tr_buffer = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getTransformMatricesSize(*DefaultConf::renderer, _debug_pipeline),
		Object3D::getTransformMatrices(*DefaultConf::renderer, _debug_pipeline).data()
	);

	// materials
	_materials_buffer = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialsSize(*DefaultConf::renderer, _debug_pipeline),
		Object3D::getMaterials(*DefaultConf::renderer, _debug_pipeline).data()
	);


	GraphicsPipelineConfiguration conf = DefaultConf::configuration;
	conf.polygon_mode = VK_POLYGON_MODE_LINE;
	conf.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	conf.line_width = 3.0f;

	_debug_pipeline = GraphicsPipeline(
		_debug_shader,
		conf
	);

	// Push Constant
	_camera = DefaultConf::camera->getMVPC();
	_debug_pipeline.setPushConstants(&_camera[0][0]);
	_debug_pipeline.setVertexBuffer("coord", _coord_buffer);
	_debug_pipeline.setVertexBuffer("object_id", _object_id_buffer);
	_debug_pipeline.setVertexBuffer("material_id", _material_indices_buffer);
	_debug_pipeline.setIndexBuffer(_index_buffer);
	_debug_pipeline.setStorageBuffer("obj_tr", _obj_tr_buffer);
	_debug_pipeline.setStorageBuffer("MaterialsBuffer", _materials_buffer);

	_debug_pipeline.update();
}

void MediapipeAndGLTF::createHandShader() {
	_hand_shader = GraphicShader(
		*DefaultConf::logical_device->getDevice(),
		"../../shaders/hand/vert.vert",
		"../../shaders/hand/frag.frag"
	);

	_hand_shader.configureVertexBuffer(
		"coord",
		0,
		VK_FORMAT_R32G32B32_SFLOAT,
		Object3D::getCoordsStride(_fake_renderer, _hand_pipeline)
	);

	_hand_shader.configureVertexBuffer(
		"joints",
		1,
		VK_FORMAT_R32G32B32A32_UINT,
		Object3D::getJointsStride(_fake_renderer, _hand_pipeline)
	);

	_hand_shader.configureVertexBuffer(
		"weights",
		2,
		VK_FORMAT_R32G32B32A32_SFLOAT,
		Object3D::getWeightsStride(_fake_renderer, _hand_pipeline)
	);

	_hand_shader.configureIndexBuffer(
		Object3D::getIndicesNbElem(_fake_renderer, _hand_pipeline)
	);

	_hand_shader.configurePushConstant(
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(_camera)
	);

	_hand_shader.configureStorageBuffer(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	_hand_shader.configureStorageBuffer(
		"joint_tr",
		2,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	_coord_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getCoordsSize(_fake_renderer, _hand_pipeline),
		Object3D::getCoords(_fake_renderer, _hand_pipeline).data()
	);

	_joints_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getJointsSize(_fake_renderer, _hand_pipeline),
		Object3D::getJoints(_fake_renderer, _hand_pipeline).data()
	);

	_weights_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getWeightsSize(_fake_renderer, _hand_pipeline),
		Object3D::getWeights(_fake_renderer, _hand_pipeline).data()
	);

	_index_hand = StagingBuffer<IGEBufferUsage::index_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getIndicesSize(_fake_renderer, _hand_pipeline),
		Object3D::getIndices(_fake_renderer, _hand_pipeline).data()
	);

	_obj_tr_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getTransformMatricesSize(_fake_renderer, _hand_pipeline),
		Object3D::getTransformMatrices(_fake_renderer, _hand_pipeline).data()
	);

	_joint_tr_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getJointsTransformSize(_fake_renderer, _hand_pipeline),
		Object3D::getJointsTransform(_fake_renderer, _hand_pipeline).data()
	);

	GraphicsPipelineConfiguration conf = DefaultConf::configuration;
	conf.polygon_mode = VK_POLYGON_MODE_LINE;
	conf.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	conf.line_width = 1.0;
	_hand_pipeline = GraphicsPipeline(
		_hand_shader,
		conf
	);

	_camera = DefaultConf::camera->getMVPC();
	_hand_pipeline.setPushConstants(&_camera[0][0]);
	_hand_pipeline.setVertexBuffer("coord", _coord_hand);
	_hand_pipeline.setVertexBuffer("joints", _joints_hand);
	_hand_pipeline.setVertexBuffer("weights", _weights_hand);
	_hand_pipeline.setIndexBuffer(_index_hand);
	_hand_pipeline.setStorageBuffer("obj_tr", _obj_tr_hand);
	_hand_pipeline.setStorageBuffer("joint_tr", _joint_tr_hand);

	_hand_pipeline.update();
}

void MediapipeAndGLTF::createCompImageSumShader() {
	_image_sum_shader = ComputeShader(
		*DefaultConf::logical_device->getDevice(),
		"../../shaders/hand/addition.comp"
	);

	_image_sum_shader.configureStorageTexture2D(
		"video_img",
		0,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);

	_image_sum_shader.configureStorageTexture2D(
		"rendered_img",
		1,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);

	_image_sum_shader.configureStorageTexture2D(
		"sum_img",
		2,
		VK_SHADER_STAGE_COMPUTE_BIT,
		1
	);

	_image_sum_shader.configurePushConstant(
		VK_SHADER_STAGE_COMPUTE_BIT,
		0,
		sizeof(_img_sum_pc)
	);

	

	_video_img = Texture2D(
		DefaultConf::logical_device->getDevice(),
		DefaultConf::render_window_width,
		DefaultConf::render_window_height,
		IGEImgFormat::r8g8b8a8_uint
	);

	_rendered_img = Texture2D(
		DefaultConf::logical_device->getDevice(),
		DefaultConf::render_window_width,
		DefaultConf::render_window_height,
		IGEImgFormat::r8g8b8a8_uint
	);

	_sum_img = Texture2D(
		DefaultConf::logical_device->getDevice(),
		DefaultConf::render_window_width,
		DefaultConf::render_window_height,
		IGEImgFormat::r8g8b8a8_uint
	);

	DefaultConf::graphics_queue->changeLayout(
		_video_img,
		VK_IMAGE_LAYOUT_GENERAL
	);

	DefaultConf::graphics_queue->changeLayout(
		_rendered_img,
		VK_IMAGE_LAYOUT_GENERAL
	);

	DefaultConf::graphics_queue->changeLayout(
		_sum_img,
		VK_IMAGE_LAYOUT_GENERAL
	);
	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();

	_image_sum_pipeline = ComputePipeline(_image_sum_shader);
	
	std::vector<Texture2D*> video_img = { &_video_img };
	std::vector<Texture2D*> rendered_img = { &_rendered_img };
	std::vector<Texture2D*> sum_img = { &_sum_img };
	_image_sum_pipeline.setTextures2D("video_img", video_img);
	_image_sum_pipeline.setTextures2D("rendered_img", rendered_img);
	_image_sum_pipeline.setTextures2D("sum_img", sum_img);
	_image_sum_pipeline.setPushConstants(&_img_sum_pc);
	
	_image_sum_pipeline.update();
}

void MediapipeAndGLTF::menu() {
	static bool is_first_loop = true;

	if (is_first_loop) {
		ImGui::SetNextWindowPos({ 300, 50 });
		ImGui::SetNextWindowSize({ 250, 250 });
		is_first_loop = false;
	}
	ImGui::Begin("Background Manager");

	ImGui::Text("Display background");
	ImGui::SliderFloat("intensity", &_img_sum_pc.intensity, 0, 1.0f);
	ImGui::Text("Chroma keying");
	ImGui::ColorPicker3("color", &_img_sum_pc.color_chroma_key.x);
	ImGui::SliderFloat("hardness", &_img_sum_pc.hardness, 0, 1.0f);

	ImGui::End();
}