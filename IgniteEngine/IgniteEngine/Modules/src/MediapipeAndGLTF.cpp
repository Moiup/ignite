#include "MediapipeAndGLTF.h"

#define IS_NETWORK 0
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
	_hand.setRenderer(DefaultConf::renderer);
	_hand.addShader(&_lbs_shader);

	_hand.setScaleLocale(glm::vec3(SCALE));

	std::cout << *_hand.getSkeleton() << std::endl;

	_red_sphere_info.loadWavefont("../../assets/3d_objects/spheres/red_sphere.obj");
	_blue_sphere_info.loadWavefont("../../assets/3d_objects/spheres/blue_sphere.obj");


	SkeletonDebug::createCrossMesh(_cross_mesh, 1.0f);
	SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

	_hand_skeleton.setObject3D(_hand);
	_hand_skeleton.setSize(1 / SCALE);
	_hand_skeleton.setShader(_debug_shader);
	_hand_skeleton.create();

	_hand_blue_sphere.resize(_hand.getSkeleton()->joints().size());
	for (const Joint& j : _hand.getSkeleton()->joints()) {
//		Joint& j = _hand.getSkeleton()->joints()[0] {
			for (const Entity3D* child : j.getChildren()) {
			const Joint* cj = static_cast<const Joint*>(child);
			_hand_blue_sphere[j.id()].addChild(&_hand_blue_sphere[cj->id()]);
		}

		_hand_blue_sphere[j.id()].createFromObjectInfo(_blue_sphere_info);
		_hand_blue_sphere[j.id()].setRenderer(DefaultConf::renderer);
		_hand_blue_sphere[j.id()].addShader(DefaultConf::graphic_shader);

		_hand_blue_sphere[j.id()].setPositionLocale(j.getPositionLocale());
		_hand_blue_sphere[j.id()].setRotationLocale(j.getRotationLocale());
		_hand_blue_sphere[j.id()].setScaleLocale(j.getScaleLocale());

	}
	_parent_hand_blue_sphere.addChild(&_hand_blue_sphere[0]);
	_parent_hand_blue_sphere.setScaleLocale(_hand.getScaleLocale());

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
		_mediapipe_info._hierarchy,
		_landmarks,
		_lfs
	);

	retargeting(_lfs, *_hand.getSkeleton());

	_mediapipe_red_sphere.resize(mdph::NB_JOINTS_LFS);
	_mediapipe_cross.resize(mdph::NB_JOINTS_LFS);
	for (uint32_t i = 0; i < mdph::NB_JOINTS_LFS; ++i) {
		_mediapipe_red_sphere[i].createFromObjectInfo(_red_sphere_info);
		_mediapipe_red_sphere[i].setRenderer(DefaultConf::renderer);
		_mediapipe_red_sphere[i].addShader(DefaultConf::graphic_shader);
		//_mediapipe_red_sphere[i].setScaleLocale(glm::vec3(SCALE));
		_mediapipe_red_sphere[i].setPositionLocale(posGlobalesMediapipe[i]);

		// Crosses
		_mediapipe_cross[i].setMesh(&_cross_mesh);
		_mediapipe_cross[i].setMaterial(_cross_material, &_cross_material_indices);
		//_mediapipe_cross[i].setRenderer(DefaultConf::renderer);
		//_mediapipe_cross[i].addShader(&_debug_shader);
		_mediapipe_cross[i].setPositionLocale(posGlobalesMediapipe[i]);
	}

	for (int i = 0; i < 21; ++i) {
		std::cout << "landmarks     : " << makeString(posGlobalesMediapipe[i]) << std::endl;
	}


	//std::cout << "wrist         : " << makeString(_hand.getSkeleton()->skeleton()->getPositionLocale()) << std::endl;
	//std::cout << "hand          : " << makeString(_hand.getPositionLocale()) << std::endl;

#endif

#endif

	createDebugShader();
	createLBSShader();
	_lbs_shader.setPolygonMode(VK_POLYGON_MODE_LINE);
	_debug_shader.setPolygonMode(VK_POLYGON_MODE_LINE);
	_debug_shader.setTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
}

void MediapipeAndGLTF::update() {
	Module::update();
	_camera = DefaultConf::camera->getMVPC();

	_obj_tr_buffer_hand.setValues(
		Object3D::updateTransformMatrices(DefaultConf::renderer, &_lbs_shader).data()
	);
	_joint_tr_buffer_hand.setValues(
		Object3D::updateJointsTransform(DefaultConf::renderer, &_lbs_shader).data()
	);


	for (const Joint& j : _hand.getSkeleton()->joints()) {
		_hand_blue_sphere[j.id()].setPositionLocale(j.getPositionLocale());
		_hand_blue_sphere[j.id()].setRotationLocale(j.getRotationLocale());
		_hand_blue_sphere[j.id()].setScaleLocale(j.getScaleLocale());
		_hand_blue_sphere[j.id()].alignmentMatrix() = j.alignmentMatrix();
	}

	_hand_skeleton.update();

	_obj_tr_buffer.setValues(
		Object3D::updateTransformMatrices(DefaultConf::renderer, &_debug_shader).data()
	);
}

void MediapipeAndGLTF::close() {
	Module::close();

	_network_thread.join();

	_mediapipe_stream.Close();

}

void MediapipeAndGLTF::networkInit() {
	_socket = EServerSocket(SERVER_ADDRESS, SERVER_PORT);
	_socket.Initialise();
	//_server_socket.Port() = SERVER_PORT;
	//_server_socket.Address() = SERVER_ADDRESS;
	//_server_socket.Initialise();
	//_server_socket.Bind();
	//_server_socket.Listen();

	//std::cout << "Listening on " << SERVER_ADDRESS << " -- " << SERVER_PORT << std::endl;
	//std::cout << "Waiting for mediapipe client to connect...";
	//_mediapipe_stream = _server_socket.Accept();
	//std::cout << "connected!" << std::endl;

	std::cout << "Connecting to " << SERVER_ADDRESS << " -- " << SERVER_PORT << std::endl;
	_mediapipe_stream = _socket.Connect();
	std::cout << "Connected!" << std::endl;

	mdph::DimMsg dim_msg;
	_mediapipe_stream.Recv(sizeof(dim_msg), reinterpret_cast<char*>(&dim_msg));
	std::cout << "received: " << std::endl;
	std::cout << "    width: " << dim_msg._width << std::endl;
	std::cout << "    height: " << dim_msg._height << std::endl;

	DefaultConf::render_window_width = dim_msg._width * 2;
	DefaultConf::render_window_height = dim_msg._height * 2;
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
	}
}

void MediapipeAndGLTF::retargeting(
	const mdph::LandmarksForSkinning& lfs,
	Skeleton& skeleton
) {
	static bool first = true;
	posGlobalesMediapipe.resize(_hand.getSkeleton()->joints().size());
	if (first) {
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
			std::cout << "PosLocale [" << id << "] = " << makeString(posLocales[id]) << std::endl;
		}
		for (auto& joint : _hand.getSkeleton()->joints())
		{
			int id = joint.id();
			glm::vec3 posLocaleGLTF = joint.getPositionLocale();
			if (joint.getParent())
			{
				int parentid = static_cast<Joint*>(joint.getParent())->id();
				glm::vec3 dir = glm::normalize(posLocales[id]);
				posGlobalesMediapipe[id] = posGlobalesMediapipe[parentid] + dir * glm::length(posLocaleGLTF) * SCALE;
			}
			else
				posGlobalesMediapipe[id] = posLocaleGLTF;
			std::cout << "PosGlobale [" << id << "] = " << makeString(posGlobalesMediapipe[id]) << std::endl;
		}
				

		first = false;
	}
	Joint* wrist = skeleton.skeleton();
	
	// Wrist case is apart
	{
		glm::vec3 from = glm::vec4(wrist->getChildren()[3]->getPositionLocale() * SCALE, 0.0f);
		glm::vec3 to = glm::inverse(wrist->getTranslateLocale() * wrist->getRotateLocale()) * glm::vec4(posGlobalesMediapipe[1],0.0f);

		wrist->alignmentMatrix() = findRotationMatrix(from, to);
	}

	std::cout << glm::length(wrist->alignmentMatrix() * glm::vec4(1, 0, 0, 0)) << std::endl;
	std::cout << glm::length(wrist->alignmentMatrix() * glm::vec4(0, 1, 0, 0)) << std::endl;
	std::cout << glm::length(wrist->alignmentMatrix() * glm::vec4(0, 0, 1, 0)) << std::endl;
#if 1
	for (int32_t parent_i = 0; parent_i < wrist->getChildren().size(); ++parent_i) {
		Joint* cur = static_cast<Joint*>(wrist->getChildren()[parent_i]);
		Joint* next = cur;
		//glm::mat4 tr = wrist->getTransformLocale();  // l2p
		glm::mat4 tr = wrist->getTranslateLocale() * wrist->getRotateLocale() * wrist->alignmentMatrix();
		std::cout << glm::length(tr * glm::vec4(1, 0, 0, 0)) << std::endl;
		std::cout << glm::length(tr * glm::vec4(0, 1, 0, 0)) << std::endl;
		std::cout << glm::length(tr * glm::vec4(0, 0, 1, 0)) << std::endl;

		for (int i = 0; i < mdph::NB_JOINTS_LFS; i++ )
			std::cout << makeString(posGlobalesMediapipe[i]) << std::endl;


		while (cur->getChildren().size()) {
			next = static_cast<Joint*>(cur->getChildren()[0]);

			//glm::mat4 tr_l2w = tr * cur->getTransformLocale();
			glm::mat4 tr_l2w = tr * cur->getTranslateLocale() * cur->getRotateLocale();

			glm::vec4 from = glm::vec4(next->getPositionLocale() * SCALE, 0.0f);
			std::cout << makeString(next->getTransform()) << std::endl;

			std::cout << cur->id() << " -- " << next->id() << std::endl;

			glm::vec4 to_point = glm::vec4(posGlobalesMediapipe[next->id()], 1.0);
			glm::vec4 toparent_point = glm::vec4(posGlobalesMediapipe[cur->id()], 1.0);
			glm::vec3 to = glm::inverse(tr_l2w) * (to_point-toparent_point);
			std::cout << "gltf length: " << glm::length(next->getPositionLocale() * SCALE) << std::endl;
			std::cout << "mediapipe length: " << glm::length((to_point-toparent_point)) << std::endl;
			std::cout << "mediapipe length: " << glm::length(to) << std::endl;

			static int i = 0;
			std::cout << i << std::endl;
			std::cout << "tr_l2w=" << makeString(tr_l2w) << std::endl;
			std::cout << "from=" << makeString(from) << std::endl;
			std::cout<<"to=" << makeString(to) << std::endl;
			std::cout << "to_point_w=" << makeString(to_point) << std::endl;
			i++;

			cur->alignmentMatrix() = findRotationMatrix(from, to);

			std::cout << "From : " << makeString(from) << std::endl;
			std::cout << "From : " << makeString(cur->alignmentMatrix() * from) << std::endl;
			std::cout << "To : " << makeString(to) << std::endl;
			tr = tr * cur->getTranslateLocale() * cur->getRotateLocale() * cur->alignmentMatrix();

			cur = next;
		}
	}
#endif
}

void MediapipeAndGLTF::createWrist(
	const mdph::Hierarchy& mediapipe_h,
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
	_lbs_shader.setNbFrame(DefaultConf::NB_FRAME);
	_lbs_shader.setDevice(DefaultConf::logical_device->getDevice());
	_lbs_shader.read(
		"../../shaders/lbs.vert",
		"../../shaders/lbs.frag"
	);
	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_lbs_shader.addVertexBufferInfo(
		"coord",
		Object3D::getCoordsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	);
	_lbs_shader.addVertexBufferInfo(
		"object_id",
		Object3D::getMeshOffsetsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	_lbs_shader.addVertexBufferInfo(
		"material_id",
		Object3D::getMaterialIndicesStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32_UINT,
		2
	);
	_lbs_shader.addVertexBufferInfo(
		"uv",
		Object3D::getUVStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32_SFLOAT,
		3
	);
	_lbs_shader.addVertexBufferInfo(
		"joints",
		Object3D::getJointsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32B32A32_UINT,
		4
	);
	_lbs_shader.addVertexBufferInfo(
		"weights",
		Object3D::getWeightsStride(DefaultConf::renderer, &_lbs_shader),
		VK_FORMAT_R32G32B32A32_SFLOAT,
		5
	);

	// Index Buffer
	_lbs_shader.addIndexBufferInfo(
		"index",
		Object3D::getIndicesNbElem(DefaultConf::renderer, &_lbs_shader)
	);

	// Push constant
	_lbs_shader.addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(_camera)
	);

	// Storage Buffers
	// transform
	_lbs_shader.addStorageBufferInfo(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	_lbs_shader.addStorageBufferInfo(
		"joint_tr",
		2,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_lbs_shader.addStorageBufferInfo(
		"MaterialsBuffer",
		3,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_lbs_shader.addSamplerInfo(
		"samp",
		4,
		VK_SHADER_STAGE_FRAGMENT_BIT
	);

	_lbs_shader.addTextureInfo(
		"textures",
		5,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		Object3D::getTextures2D(DefaultConf::renderer, &_lbs_shader).size()
	);


	//----------------------//
	// Creating the buffers //
	//----------------------//
	_coord_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getCoordsSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getCoords(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addVertexBuffer("coord", &_coord_buffer_hand);

	_object_id_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getObjectIdSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getObjectId(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addVertexBuffer("object_id", &_object_id_buffer_hand);

	_material_indices_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialIndicesSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getMaterialIndices(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addVertexBuffer("material_id", &_material_indices_buffer_hand);

	_uv_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getUVSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getUV(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addVertexBuffer("uv", &_uv_buffer_hand);

	_joints_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getJointsSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getJoints(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addVertexBuffer("joints", &_joints_buffer_hand);

	_weights_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getWeightsSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getWeights(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addVertexBuffer("weights", &_weights_buffer_hand);

	// Index buffer
	_index_buffer_hand = StagingBuffer<IGEBufferUsage::index_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getIndicesSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getIndices(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addIndexBuffer("index", &_index_buffer_hand);

	// Push Constant
	_lbs_shader.addPushConstant("pc", &_camera[0][0]);

	// Storage Buffers
	// transform
	_obj_tr_buffer_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getTransformMatricesSize(DefaultConf::renderer, &_lbs_shader),
		&Object3D::getTransformMatrices(DefaultConf::renderer, &_lbs_shader)[0][0]
	);
	_lbs_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer_hand);

	// Joint transform
	_joint_tr_buffer_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getJointsTransformSize(DefaultConf::renderer, &_lbs_shader),
		&Object3D::getJointsTransform(DefaultConf::renderer, &_lbs_shader)[0][0]
	);
	_lbs_shader.addStorageBuffer("joint_tr", &_joint_tr_buffer_hand);

	// materials
	_materials_buffer_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialsSize(DefaultConf::renderer, &_lbs_shader),
		Object3D::getMaterials(DefaultConf::renderer, &_lbs_shader).data()
	);
	_lbs_shader.addStorageBuffer("MaterialsBuffer", &_materials_buffer_hand);

	// Sampler
	_sampler_hand.setDevice(DefaultConf::logical_device->getDevice());
	_sampler_hand.create();
	_lbs_shader.addSampler("samp", &_sampler_hand);

	// Textures
	_lbs_shader.addTexture2D(
		"textures",
		Object3D::getTextures2D(DefaultConf::renderer, &_lbs_shader)
	);
}

void MediapipeAndGLTF::createDebugShader() {
	_debug_shader.setNbFrame(DefaultConf::NB_FRAME);
	_debug_shader.setDevice(DefaultConf::logical_device->getDevice());
	_debug_shader.read(
		"../../shaders/debug/debug.vert",
		"../../shaders/debug/debug.frag"
	);
	// -- Vertex Shader -- //
	// Configuring the Graphic Shader
	_debug_shader.addVertexBufferInfo(
		"coord",
		Object3D::getCoordsStride(DefaultConf::renderer, &_debug_shader),
		VK_FORMAT_R32G32B32_SFLOAT,
		0
	);
	_debug_shader.addVertexBufferInfo(
		"object_id",
		Object3D::getMeshOffsetsStride(DefaultConf::renderer, &_debug_shader),
		VK_FORMAT_R32_UINT,
		1
	);
	_debug_shader.addVertexBufferInfo(
		"material_id",
		Object3D::getMaterialIndicesStride(DefaultConf::renderer, &_debug_shader),
		VK_FORMAT_R32_UINT,
		2
	);

	// Index Buffer
	_debug_shader.addIndexBufferInfo(
		"index",
		Object3D::getIndicesNbElem(DefaultConf::renderer, &_debug_shader)
	);

	// Push constant
	_debug_shader.addPushConstantInfo(
		"pc",
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(DefaultConf::camera->getMVPC())
	);

	// Storage Buffers
// transform
	_debug_shader.addStorageBufferInfo(
		"obj_tr",
		1,
		VK_SHADER_STAGE_VERTEX_BIT
	);

	// -- Fragment shader -- //
	// materials
	_debug_shader.addStorageBufferInfo(
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
		Object3D::getCoordsSize(DefaultConf::renderer, &_debug_shader),
		Object3D::getCoords(DefaultConf::renderer, &_debug_shader).data()
	);
	_debug_shader.addVertexBuffer("coord", &_coord_buffer);

	_object_id_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getObjectIdSize(DefaultConf::renderer, &_debug_shader),
		Object3D::getObjectId(DefaultConf::renderer, &_debug_shader).data()
	);
	_debug_shader.addVertexBuffer("object_id", &_object_id_buffer);

	_material_indices_buffer = StagingBuffer<IGEBufferUsage::vertex_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialIndicesSize(DefaultConf::renderer, &_debug_shader),
		Object3D::getMaterialIndices(DefaultConf::renderer, &_debug_shader).data()
	);
	_debug_shader.addVertexBuffer("material_id", &_material_indices_buffer);


	// Index buffer
	_index_buffer = StagingBuffer<IGEBufferUsage::index_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getIndicesSize(DefaultConf::renderer, &_debug_shader),
		Object3D::getIndices(DefaultConf::renderer, &_debug_shader).data()
	);
	_debug_shader.addIndexBuffer("index", &_index_buffer);

	// Push Constant
	_camera = DefaultConf::camera->getMVPC();
	_debug_shader.addPushConstant("pc", &_camera[0][0]);


	// Storage Buffers
	// transform
	_obj_tr_buffer = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getTransformMatricesSize(DefaultConf::renderer, &_debug_shader),
		Object3D::getTransformMatrices(DefaultConf::renderer, &_debug_shader).data()
	);
	_debug_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);

	// materials
	_materials_buffer = StagingBuffer<IGEBufferUsage::storage_buffer>(
		DefaultConf::logical_device->getDevice(),
		Object3D::getMaterialsSize(DefaultConf::renderer, &_debug_shader),
		Object3D::getMaterials(DefaultConf::renderer, &_debug_shader).data()
	);
	_debug_shader.addStorageBuffer("MaterialsBuffer", &_materials_buffer);
}