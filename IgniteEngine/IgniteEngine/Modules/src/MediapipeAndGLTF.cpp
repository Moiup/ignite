#include "MediapipeAndGLTF.h"

#define IS_NETWORK 0
#define IS_GLTF 1

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
	readMediapipeFile("../../assets/mediapipe_data/mp_index_moved.txt");
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

	//std::cout << *_hand.getSkeleton() << std::endl;

	_cube_info.loadWavefont("../../assets/3d_objects/cube_textured/cube_tex.obj");

	//SkeletonDebug::createCrossMesh(_cross_mesh, 0.1);
	//SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

	_hand.setScaleLocale(glm::vec3(0.25f));

	SkeletonDebug::createCrossMesh(_cross_mesh, 1.0f);
	SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

	_hand_skeleton.setObject3D(_hand);
	_hand_skeleton.setSize(3.0f);
	_hand_skeleton.setShader(_debug_shader);
	_hand_skeleton.create();

	_right_hand.resize(_hand.getSkeleton()->joints().size());
	_alignment_matrices.resize(_right_hand.size());
	std::fill(
		_alignment_matrices.begin(),
		_alignment_matrices.end(),
		glm::mat4(1.0)
	);
	_transform_matrices.resize(_right_hand.size());
	std::fill(
		_transform_matrices.begin(),
		_transform_matrices.end(),
		glm::mat4(1.0)
	);



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
	for (uint32_t i = 0; i < _right_hand.size(); ++i) {
		_landmarks._landmarks[0][i] = glm::vec3(
			_landmarks._landmarks[0][i].x,
			_landmarks._landmarks[0][i].y,
			_landmarks._landmarks[0][i].z
		) * 20.0f;
		_right_hand[i].createFromObjectInfo(_cube_info);
		_right_hand[i].setRenderer(DefaultConf::renderer);
		_right_hand[i].addShader(DefaultConf::graphic_shader);
		_right_hand[i].setScaleLocale(glm::vec3(0.25));
		_right_hand[i].setPositionLocale(
			_landmarks._landmarks[0][i].x,
			_landmarks._landmarks[0][i].y,
			_landmarks._landmarks[0][i].z
		);
	}
	landmarksRotationMatrices(_landmarks);
	//landmarksLookAtMatrices(_landmarks);

	for (int i = 0; i < 21; ++i) {
		std::cout << "landmarks     : " << makeString(_landmarks._landmarks[0][i]) << std::endl;
		//std::cout << "_right_hand[" << i << "]: " << makeString(_right_hand[i].getPositionLocale()) << std::endl;
	}
	std::cout << "wrist         : " << makeString(_hand.getSkeleton()->skeleton()->getPositionLocale()) << std::endl;
	std::cout << "hand          : " << makeString(_hand.getPositionLocale()) << std::endl;

#endif

#endif

	_lbs_shader.setPolygonMode(VK_POLYGON_MODE_LINE);
	_debug_shader.setPolygonMode(VK_POLYGON_MODE_LINE);
	_debug_shader.setTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
	createShaderHand();
	createDebugShader();
}

void MediapipeAndGLTF::update() {
	Module::update();

	//const Joint* wrist = _hand.getSkeleton()->skeleton();
	//glm::mat4 wrist_tr = _alignment_matrices[wrist->id()] * wrist->getTransformLocale();
	//_transform_matrices[wrist->id()] = wrist_tr * wrist->inverseBindMatrices();
	//for (int i = 0; i < 5; ++i) {
	//    const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[i]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	//    const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	//    const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	//    const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);

	//    glm::mat4 tr = wrist_tr * _alignment_matrices[wrist_bis->id()] * wrist_bis->getTransformLocale();
	//    _transform_matrices[wrist_bis->id()] = tr * wrist_bis->inverseBindMatrices();

	//    tr = tr * _alignment_matrices[phal1->id()] * phal1->getTransformLocale();
	//    _transform_matrices[phal1->id()] = tr * phal1->inverseBindMatrices();

	//    tr = tr * _alignment_matrices[phal2->id()] * phal2->getTransformLocale();
	//    _transform_matrices[phal2->id()] = tr * phal2->inverseBindMatrices();

	//    tr = tr * _alignment_matrices[phal3->id()] * phal3->getTransformLocale();
	//    _transform_matrices[phal3->id()] = tr * phal3->inverseBindMatrices();

	//    //tr = tr * phal4->getTransformLocale();
	//    //_transform_matrices[phal4->id()] = tr * phal4->inverseBindMatrices();
	//}

	const Joint* wrist = _hand.getSkeleton()->skeleton();
	glm::mat4 wrist_tr = wrist->getTransformLocale() * _alignment_matrices[wrist->id()];
	_transform_matrices[wrist->id()] = wrist_tr * wrist->inverseBindMatrices();
	for (int i = 0; i < 5; ++i) {
		const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[i]);
		const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
		const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
		const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);

		glm::mat4 tr = wrist_tr * wrist_bis->getTransformLocale() * _alignment_matrices[wrist_bis->id()];
		_transform_matrices[wrist_bis->id()] = tr * wrist_bis->inverseBindMatrices();

		tr = tr * phal1->getTransformLocale() * _alignment_matrices[phal1->id()];
		_transform_matrices[phal1->id()] = tr * phal1->inverseBindMatrices();

		tr = tr * phal2->getTransformLocale() * _alignment_matrices[phal2->id()];
		_transform_matrices[phal2->id()] = tr * phal2->inverseBindMatrices();

		tr = tr * phal3->getTransformLocale() * _alignment_matrices[phal3->id()];
		_transform_matrices[phal3->id()] = tr * phal3->inverseBindMatrices();
	}

	_camera = DefaultConf::camera->getMVPC();

	_joint_tr_buffer_hand.setValues(
		_transform_matrices.data()
	);

	//_obj_tr_buffer.setValues(
	//    _transform_matrices.data()
	//);
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


		//landmarksToLocal(landmarks);



		landmarksRotationMatrices(landmarks);
		//landmarksLookAtMatrices(landmarks);
	}
}

void MediapipeAndGLTF::readMediapipeFile(const std::string& path) {
	std::ifstream mp_file = std::ifstream(path);

	if (!mp_file.is_open()) {
		std::cerr << "MediapipeAndGLTF::readMediapipeFile: Error opening file '" + path + "'." << std::endl;
		throw std::runtime_error("MediapipeAndGLTF::readMediapipeFile: Error opening file '" + path + "'.");
	}

	// -- Reading dimensions --
	int32_t width;
	int32_t height;

	mp_file >> width;
	mp_file >> height;

	DefaultConf::render_window_width = width * 2;
	DefaultConf::render_window_height = height * 2;

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

void MediapipeAndGLTF::landmarksRotationMatrices(const mdph::Landmarks& landmarks) {
	// Let us work with the index finger
	// Let us consider that the position of a child, is (0, 1, 0) to the father
	//_hand.setPositionLocale(landmarks._landmarks[0][0]);
	//_hand.setPositionLocale(landmarks._landmarks[0][0]);
	//_hand.setRotationLocale(-90, 0, 0);

	std::cout << "landmarksRotationMatrices" << std::endl;

	Joint* wrist = _hand.getSkeleton()->skeleton();
	//wrist->setPositionLocale(landmarks._landmarks[0][0]);
	//{
	//    Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[2]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);

	//    glm::mat4 m1 = wrist->getTransformLocale();

	//    glm::vec3 v_m = glm::inverse(m1) * (m1 * glm::vec4(0, 0, 0, 1) + glm::vec4(landmarks._landmarks[0][9] - landmarks._landmarks[0][0], 0.0f));

	//    glm::vec3 v_s =
	//        glm::inverse(m1)
	//        * wrist->getTransformLocale()
	//        * wrist_bis->getTransformLocale()
	//        * glm::vec4(phal1->getPositionLocale(), 1);

	//    _alignment_matrices[wrist->id()] = findRotationMatrix(v_s, v_m);

	//    std::cout << makeString(glm::normalize(v_s)) << std::endl;
	//    std::cout << makeString(glm::normalize(v_m)) << std::endl;

	//    std::cout << "_alignment_matrices[wrist->id()]" << std::endl;
	//    std::cout << makeString(_alignment_matrices[wrist->id()]) << std::endl;

	//}
	{
		Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[2]);
		const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
		glm::vec3 v_m = landmarks._landmarks[0][9] - landmarks._landmarks[0][0];
		glm::vec3 v_s = wrist->getTransformLocale() * wrist_bis->getTransformLocale() * glm::vec4(phal1->getPositionLocale(), 1);
		_alignment_matrices[wrist->id()] = findRotationMatrix(v_s, v_m);

		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "wrist" << std::endl;
		std::cout << makeString(v_s) << std::endl;
		std::cout << makeString(glm::normalize(_alignment_matrices[wrist->id()] * glm::vec4(v_s, 1.0))) << std::endl;
		std::cout << makeString(glm::normalize(v_m)) << std::endl;

		std::cout << "alignment matrix" << std::endl;
		std::cout << makeString(_alignment_matrices[wrist->id()]) << std::endl;

		std::cout << std::endl;
		std::cout << std::endl;

	}

	for (int32_t i = 0; i < 5; i++) {
		int32_t land_i = i * 4;
		fingerAlignement(
			landmarks,
			wrist,
			static_cast<Joint*>(wrist->getChildren()[i]),
			landmarks._landmarks[0][land_i + 1],
			landmarks._landmarks[0][land_i + 2],
			landmarks._landmarks[0][land_i + 3],
			landmarks._landmarks[0][land_i + 4]
		);
	}


	//// Thumb
	//fingerAlignement(
	//    landmarks,
	//    wrist,
	//    static_cast<Joint*>(wrist->getChildren()[0]),
	//    landmarks._landmarks[0][1] - landmarks._landmarks[0][0],
	//    landmarks._landmarks[0][2] - landmarks._landmarks[0][1],
	//    landmarks._landmarks[0][3] - landmarks._landmarks[0][2],
	//    landmarks._landmarks[0][4] - landmarks._landmarks[0][3]
	//);
	//// Index
	//fingerAlignement(
	//	landmarks,
	//	wrist,
	//	static_cast<Joint*>(wrist->getChildren()[1]),
	//	landmarks._landmarks[0][5],
	//	landmarks._landmarks[0][6],
	//	landmarks._landmarks[0][7],
	//	landmarks._landmarks[0][8]
	//);
	//// Middle
	//fingerAlignement(
	//    landmarks,
	//    wrist,
	//    static_cast<Joint*>(wrist->getChildren()[2]),
	//    landmarks._landmarks[0][9],
	//    landmarks._landmarks[0][10],
	//    landmarks._landmarks[0][11],
	//    landmarks._landmarks[0][12]
	//);
	//// Ring
	//fingerAlignement(
	//    landmarks,
	//    wrist,
	//    static_cast<Joint*>(wrist->getChildren()[3]),
	//    landmarks._landmarks[0][13] - landmarks._landmarks[0][0],
	//    landmarks._landmarks[0][14] - landmarks._landmarks[0][13],
	//    landmarks._landmarks[0][15] - landmarks._landmarks[0][14],
	//    landmarks._landmarks[0][16] - landmarks._landmarks[0][15]
	//);
	//// Pincky
	//fingerAlignement(
	//    landmarks,
	//    wrist,
	//    static_cast<Joint*>(wrist->getChildren()[4]),
	//    landmarks._landmarks[0][17] - landmarks._landmarks[0][0],
	//    landmarks._landmarks[0][18] - landmarks._landmarks[0][17],
	//    landmarks._landmarks[0][19] - landmarks._landmarks[0][18],
	//    landmarks._landmarks[0][20] - landmarks._landmarks[0][19]
	//);
}

void MediapipeAndGLTF::fingerAlignement(
	const mdph::Landmarks& landmarks,
	const Joint* wrist,
	const Joint* wrist_bis,
	const glm::vec3 vm1,
	const glm::vec3 vm2,
	const glm::vec3 vm3,
	const glm::vec3 vm4
) {
	const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);

	{
		glm::vec3 target = vm1;
		glm::vec3 v_m = target;

		glm::mat4 l2w =
			wrist->getTransformLocale() * _alignment_matrices[wrist->id()]
			* wrist_bis->getTransformLocale();

		v_m = glm::inverse(l2w) * glm::vec4(v_m, 1.0);

		glm::vec3 v_s = glm::vec4(phal1->getPositionLocale(), 1.0);

		_alignment_matrices[wrist_bis->id()] = findRotationMatrix(v_s, v_m);

		glm::vec3 result =
			wrist->getTransformLocale() * _alignment_matrices[wrist->id()]
			* wrist_bis->getTransformLocale() * _alignment_matrices[wrist_bis->id()]
			* phal1->getTransformLocale()
			* glm::vec4(0, 0, 0, 1);

		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "wrist_bis:" << std::endl;
		std::cout << "v_m: " << makeString(v_m) << std::endl;
		std::cout << "v_s: " << makeString(v_s) << std::endl;
		std::cout << std::endl;
		std::cout << "normalized a * v_s: " << makeString(glm::normalize(_alignment_matrices[wrist_bis->id()] * glm::vec4(v_s, 1.0))) << std::endl;
		std::cout << "normalized v_m    : " << makeString(glm::normalize(v_m)) << std::endl;
		std::cout << std::endl;
		std::cout << "target: " << makeString(target) << std::endl;
		std::cout << "result: " << makeString(result) << std::endl;
		std::cout << "diff  : " << makeString(target - result) << std::endl;
		std::cout << std::endl;
		std::cout << "alignment matrix" << std::endl;
		std::cout << makeString(_alignment_matrices[wrist_bis->id()]) << std::endl;

		std::cout << std::endl;
		std::cout << std::endl;
	}
	{
		//glm::vec3 v_m = vm2;
		glm::vec3 target = vm2;
		glm::vec3 v_m = target;

		glm::mat4 l2w =
			wrist->getTransformLocale() * _alignment_matrices[wrist->id()]
			* wrist_bis->getTransformLocale() * _alignment_matrices[wrist_bis->id()]
			* phal1->getTransformLocale();

		v_m = glm::inverse(l2w) * glm::vec4(v_m, 1.0);

		//glm::vec3 phal2_pos =
		//	wrist->getTransformLocale()
		//	* wrist_bis->getTransformLocale()
		//	* phal1->getTransformLocale()
		//	* phal2->getTransformLocale()
		//	* glm::vec4(0, 0, 0, 1.0);

		glm::vec3 v_s = glm::vec4(phal2->getPositionLocale(), 1.0);

		_alignment_matrices[phal1->id()] = findRotationMatrix(v_s, v_m);

		glm::vec3 result =
			_alignment_matrices[wrist->id()] * wrist->getTransformLocale()
			* _alignment_matrices[wrist_bis->id()] * wrist_bis->getTransformLocale()
			* _alignment_matrices[phal1->id()] * phal1->getTransformLocale()
			* phal2->getTransformLocale()
			* glm::vec4(0, 0, 0, 1);


		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "phal1:" << std::endl;
		std::cout << "v_m: " << makeString(v_m) << std::endl;
		std::cout << "v_s: " << makeString(v_s) << std::endl;
		std::cout << std::endl;
		std::cout << "normalized a * v_s: " << makeString(glm::normalize(_alignment_matrices[phal1->id()] * glm::vec4(v_s, 1.0))) << std::endl;
		std::cout << "normalized v_m    : " << makeString(glm::normalize(v_m)) << std::endl;
		std::cout << std::endl;
		std::cout << "target: " << makeString(target) << std::endl;
		std::cout << "result: " << makeString(result) << std::endl;
		std::cout << "diff  : " << makeString(target - result) << std::endl;
		std::cout << std::endl;
		std::cout << "alignment matrix" << std::endl;
		std::cout << makeString(_alignment_matrices[phal1->id()]) << std::endl;

		std::cout << std::endl;
		std::cout << std::endl;
	}

	{
		glm::vec3 target = vm3;
		glm::vec3 v_m = target;

		glm::mat4 l2w =
			wrist->getTransformLocale() * _alignment_matrices[wrist->id()]
			* wrist_bis->getTransformLocale() * _alignment_matrices[wrist_bis->id()]
			* phal1->getTransformLocale() * _alignment_matrices[phal1->id()]
			* phal2->getTransformLocale();

		v_m = glm::inverse(l2w) * glm::vec4(v_m, 1.0);

		//glm::vec3 v_m = vm3;
		glm::vec3 v_s = glm::vec4(phal3->getPositionLocale(), 1.0);

		_alignment_matrices[phal2->id()] = findRotationMatrix(v_s, v_m);


		glm::vec3 result =
			_alignment_matrices[wrist->id()] * wrist->getTransformLocale()
			* _alignment_matrices[wrist_bis->id()] * wrist_bis->getTransformLocale()
			* _alignment_matrices[phal1->id()] * phal1->getTransformLocale()
			* phal2->getTransformLocale() * _alignment_matrices[phal2->id()]
			* phal3->getTransformLocale()
			* glm::vec4(0, 0, 0, 1);

		

		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "phal2:" << std::endl;
		std::cout << "v_m:     " << makeString(v_m) << " -- " << glm::length(v_m) << std::endl;
		std::cout << "v_s:     " << makeString(v_s) << " -- " << glm::length(v_s) << std::endl;
		std::cout << "a * v_s: " << makeString(_alignment_matrices[phal2->id()] * glm::vec4(v_s, 1.0)) << std::endl;
		std::cout << std::endl;
		std::cout << "normalized a * v_s: " << makeString(glm::normalize(_alignment_matrices[phal2->id()] * glm::vec4(v_s, 1.0))) << std::endl;
		std::cout << "normalized v_m    : " << makeString(glm::normalize(v_m)) << std::endl;
		std::cout << std::endl;
		std::cout << "target: " << makeString(target) << std::endl;
		std::cout << "result: " << makeString(result) << std::endl;
		std::cout << "diff  : " << makeString(target - result) << std::endl;
		std::cout << std::endl;
		std::cout << "alignment matrix" << std::endl;
		std::cout << makeString(_alignment_matrices[phal2->id()]) << std::endl;

		std::cout << std::endl;
		std::cout << std::endl;
	}
	{
		glm::vec3 target = vm4;
		glm::vec3 v_m = target;

		glm::mat4 l2w =
			wrist->getTransformLocale() * _alignment_matrices[wrist->id()]
			* wrist_bis->getTransformLocale() * _alignment_matrices[wrist_bis->id()]
			* phal1->getTransformLocale() * _alignment_matrices[phal1->id()]
			* phal2->getTransformLocale() * _alignment_matrices[phal2->id()]
			* phal3->getTransformLocale();

		v_m = glm::inverse(l2w) * glm::vec4(v_m, 1.0);

		//glm::vec3 v_m = vm3;
		glm::vec3 v_s = glm::vec4(phal3->getPositionLocale(), 1.0);

		_alignment_matrices[phal3->id()] = findRotationMatrix(v_s, v_m);


		glm::vec3 result =
			_alignment_matrices[wrist->id()] * wrist->getTransformLocale()
			* _alignment_matrices[wrist_bis->id()] * wrist_bis->getTransformLocale()
			* _alignment_matrices[phal1->id()] * phal1->getTransformLocale()
			* phal2->getTransformLocale() * _alignment_matrices[phal2->id()]
			* phal3->getTransformLocale() * _alignment_matrices[phal3->id()]
			* phal4->getTransformLocale()
			* glm::vec4(0, 0, 0, 1);

		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "phal3:" << std::endl;
		std::cout << "v_m:     " << makeString(v_m) << " -- " << glm::length(v_m) << std::endl;
		std::cout << "v_s:     " << makeString(v_s) << " -- " << glm::length(v_s) << std::endl;
		std::cout << "a * v_s: " << makeString(_alignment_matrices[phal3->id()] * glm::vec4(v_s, 1.0)) << std::endl;
		std::cout << std::endl;
		std::cout << "normalized a * v_s: " << makeString(glm::normalize(_alignment_matrices[phal3->id()] * glm::vec4(v_s, 1.0))) << std::endl;
		std::cout << "normalized v_m    : " << makeString(glm::normalize(v_m)) << std::endl;
		std::cout << std::endl;
		std::cout << "target: " << makeString(target) << std::endl;
		std::cout << "result: " << makeString(result) << std::endl;
		std::cout << "diff  : " << makeString(target - result) << std::endl;
		std::cout << std::endl;
		std::cout << "alignment matrix" << std::endl;
		std::cout << makeString(_alignment_matrices[phal3->id()]) << std::endl;

		std::cout << std::endl;
		std::cout << std::endl;
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

glm::vec3 MediapipeAndGLTF::vectorAngle(glm::vec3 A, glm::vec3 B) {
	float angle_z = 0;
	if (A.x * B.x + A.y * B.y > 0 || A.x * B.x + A.y * B.y < 0) {
		angle_z = std::acos(
			(A.x * B.x + A.y * B.y)
			/
			(std::sqrt(A.x * A.x + A.y * A.y) * std::sqrt(B.x * B.x + B.y * B.y))
		);
	}

	float angle_y = 0;
	if (A.x * B.x + A.z * B.z > 0 || A.x * B.x + A.z * B.z < 0) {
		angle_y = std::acos(
			(A.x * B.x + A.z * B.z)
			/
			(std::sqrt(A.x * A.x + A.z * A.z) * std::sqrt(B.x * B.x + B.z * B.z))
		);
	}

	float angle_x = 0;
	if (A.y * B.y + A.z * B.z > 0 || A.y * B.y + A.z * B.z < 0) {
		angle_x = std::acos(
			(A.y * B.y + A.z * B.z)
			/
			(std::sqrt(A.y * A.y + A.z * A.z) * std::sqrt(B.y * B.y + B.z * B.z))
		);
	}
	return glm::vec3(angle_x, angle_y, angle_z);
}

void MediapipeAndGLTF::landmarksLookAtMatrices(const mdph::Landmarks& landmarks) {
	// Let us work with the index finger
	// Let us consider that the position of a child, is (0, 1, 0) to the father
	//_hand.setPositionLocale(landmarks._landmarks[0][0]);
	//_hand.setPositionLocale(landmarks._landmarks[0][0]);
	//_hand.setRotationLocale(-90, 0, 0);
	//glm::vec3 up = glm::vec3(1, 0, 0);
	//Joint* wrist = _hand.getSkeleton()->skeleton();
	//{
	//    Joint* mid_wrist = static_cast<Joint*>(wrist->getChildren()[2]);
	//    //_hand.setPositionLocale(landmarks._landmarks[0][0]);
	//    wrist->setPositionLocale(landmarks._landmarks[0][0]);
	//    glm::vec3 eye = landmarks._landmarks[0][0];
	//    glm::vec3 center = landmarks._landmarks[0][9];
	//    glm::mat4 align = glm::lookAt(eye, center, up);
	//    //wrist->setPositionLocale(0, 100, 0);
	//    //wrist->setRotationLocale(glm::vec3(glm::radians(180.0f), glm::radians(90.0f), 0.0f));
	//    {
	//        glm::vec3 v_m = landmarks._landmarks[0][9] - landmarks._landmarks[0][0];
	//        //glm::vec3 v_s = mid_wrist->getTransformLocale() * glm::vec4(0, 0, 0, 1);
	//        _alignment_matrices[wrist->id()] = align;
	//    }
	//}

	//{
	//    const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[0]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	//    const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	//    const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	//    const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][0];
	//        glm::vec3 center = landmarks._landmarks[0][1];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[wrist_bis->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][1];
	//        glm::vec3 center = landmarks._landmarks[0][2];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal1->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][2];
	//        glm::vec3 center = landmarks._landmarks[0][3];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal2->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][3];
	//        glm::vec3 center = landmarks._landmarks[0][4];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal3->id()] = align;
	//    }
	//}

	//{
	//    const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[1]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	//    const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	//    const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	//    const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][0];
	//        glm::vec3 center = landmarks._landmarks[0][5];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[wrist_bis->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][5];
	//        glm::vec3 center = landmarks._landmarks[0][6];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal1->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][6];
	//        glm::vec3 center = landmarks._landmarks[0][7];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal2->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][7];
	//        glm::vec3 center = landmarks._landmarks[0][8];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal3->id()] = align;
	//    }
	//}

	//{
	//    const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[2]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	//    const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	//    const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	//    const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][0];
	//        glm::vec3 center = landmarks._landmarks[0][9];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[wrist_bis->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][9];
	//        glm::vec3 center = landmarks._landmarks[0][10];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal1->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][10];
	//        glm::vec3 center = landmarks._landmarks[0][11];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal2->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][11];
	//        glm::vec3 center = landmarks._landmarks[0][12];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal3->id()] = align;
	//    }
	//}

	//{
	//    const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[3]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	//    const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	//    const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	//    const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][0];
	//        glm::vec3 center = landmarks._landmarks[0][13];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[wrist_bis->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][13];
	//        glm::vec3 center = landmarks._landmarks[0][14];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal1->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][14];
	//        glm::vec3 center = landmarks._landmarks[0][15];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal2->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][15];
	//        glm::vec3 center = landmarks._landmarks[0][16];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal3->id()] = align;
	//    }
	//}

	//{
	//    const Joint* wrist_bis = static_cast<Joint*>(wrist->getChildren()[4]);
	//    const Joint* phal1 = static_cast<Joint*>(wrist_bis->getChildren()[0]);
	//    const Joint* phal2 = static_cast<Joint*>(phal1->getChildren()[0]);
	//    const Joint* phal3 = static_cast<Joint*>(phal2->getChildren()[0]);
	//    const Joint* phal4 = static_cast<Joint*>(phal3->getChildren()[0]);
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][0];
	//        glm::vec3 center = landmarks._landmarks[0][17];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[wrist_bis->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][17];
	//        glm::vec3 center = landmarks._landmarks[0][18];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal1->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][18];
	//        glm::vec3 center = landmarks._landmarks[0][19];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal2->id()] = align;
	//    }
	//    {
	//        glm::vec3 eye = landmarks._landmarks[0][19];
	//        glm::vec3 center = landmarks._landmarks[0][20];
	//        glm::mat4 align = glm::lookAt(eye, center, up);
	//        _alignment_matrices[phal3->id()] = align;
	//    }
	//}
}


void MediapipeAndGLTF::landmarksToLocal(const mdph::Landmarks& landmarks) {
	const float offset_size = 20.0f;
	_right_hand[0].setPositionLocale(landmarks._landmarks[0][0] * 10.f);
	// One
	_right_hand[1].setPositionLocale(
		(landmarks._landmarks[0][1] - landmarks._landmarks[0][0]) * offset_size
	);
	_right_hand[2].setPositionLocale(
		(landmarks._landmarks[0][2] - landmarks._landmarks[0][1]) * offset_size
	);
	_right_hand[3].setPositionLocale(
		(landmarks._landmarks[0][3] - landmarks._landmarks[0][2]) * offset_size
	);
	_right_hand[4].setPositionLocale(
		(landmarks._landmarks[0][4] - landmarks._landmarks[0][3]) * offset_size
	);
	// two
	_right_hand[5].setPositionLocale(
		(landmarks._landmarks[0][5] - landmarks._landmarks[0][0]) * offset_size
	);
	_right_hand[6].setPositionLocale(
		(landmarks._landmarks[0][6] - landmarks._landmarks[0][5]) * offset_size
	);
	_right_hand[7].setPositionLocale(
		(landmarks._landmarks[0][7] - landmarks._landmarks[0][6]) * offset_size
	);
	_right_hand[7].setPositionLocale(
		(landmarks._landmarks[0][8] - landmarks._landmarks[0][7]) * offset_size
	);
	// three
	_right_hand[9].setPositionLocale(
		(landmarks._landmarks[0][9] - landmarks._landmarks[0][0]) * offset_size
	);
	_right_hand[10].setPositionLocale(
		(landmarks._landmarks[0][10] - landmarks._landmarks[0][9]) * offset_size
	);
	_right_hand[11].setPositionLocale(
		(landmarks._landmarks[0][11] - landmarks._landmarks[0][10]) * offset_size
	);
	_right_hand[12].setPositionLocale(
		(landmarks._landmarks[0][12] - landmarks._landmarks[0][11]) * offset_size
	);
	// four
	_right_hand[13].setPositionLocale(
		(landmarks._landmarks[0][13] - landmarks._landmarks[0][0]) * offset_size
	);
	_right_hand[14].setPositionLocale(
		(landmarks._landmarks[0][14] - landmarks._landmarks[0][13]) * offset_size
	);
	_right_hand[15].setPositionLocale(
		(landmarks._landmarks[0][15] - landmarks._landmarks[0][14]) * offset_size
	);
	_right_hand[16].setPositionLocale(
		(landmarks._landmarks[0][16] - landmarks._landmarks[0][15]) * offset_size
	);
	// five
	_right_hand[17].setPositionLocale(
		(landmarks._landmarks[0][17] - landmarks._landmarks[0][0]) * offset_size
	);
	_right_hand[18].setPositionLocale(
		(landmarks._landmarks[0][18] - landmarks._landmarks[0][17]) * offset_size
	);
	_right_hand[19].setPositionLocale(
		(landmarks._landmarks[0][19] - landmarks._landmarks[0][18]) * offset_size
	);
	_right_hand[20].setPositionLocale(
		(landmarks._landmarks[0][20] - landmarks._landmarks[0][19]) * offset_size
	);
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

void MediapipeAndGLTF::createShaderHand() {
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