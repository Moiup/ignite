#include "MediapipeModule.h"

MediapipeModule::MediapipeModule() : Module::Module() {
	;
}

void MediapipeModule::init() {
	Module::init();

	_server_socket.Port() = SERVER_PORT;
	_server_socket.Address() = SERVER_ADDRESS;
	_server_socket.Initialise();
	_server_socket.Bind();
	_server_socket.Listen();
	
	std::cout << "Listening on " << SERVER_ADDRESS << " -- " << SERVER_PORT << std::endl;
	std::cout << "Waiting for mediapipe client to connect...";
	_mediapipe_stream = _server_socket.Accept();
	std::cout << "connected!" << std::endl;
}

void MediapipeModule::start() {
	Module::start();

	InitNetworkProtocole hand_info{};
	hand_info.read(_mediapipe_stream);
	
	_cube.setVertex(cube(), cubeIndex());
	
	_hands.resize(hand_info.getHeader().nb_repetitions);
	_wrist_pos.resize(hand_info.getHeader().nb_repetitions);
	for (auto& hand : _hands) {
		hand.resize(hand_info.getHeader().nb_points);
	}

	// Creating the hands
	// Creating the objects and affecting the child
	// Heach joint is represented by a cube
	for (auto& hand : _hands) {
		for (const InitChildPoint& point : hand_info.getPoints()) {
			hand[point.child_id].setMesh(&_cube);
			hand[point.child_id].setRenderer(DefaultConf::renderer);
			hand[point.child_id].setScaleAbsolute(0.1, 0.1, 0.1);

			if (point.child_id == point.parent_id) {
				continue;
			}

			hand[point.parent_id].addChild(&hand[point.child_id]);
		}
	}

	char ok = 1;
	_mediapipe_stream.Send(&ok, sizeof(ok));
	_mediapipe_stream.Recv(sizeof(ok), &ok);

	if (ok) {
		_mediapipe_stream.Send(&ok, sizeof(ok));
	}

	_frame_i = 0;
}

void MediapipeModule::update() {
	Module::update();

	MocapMessageProtocolNetwork msg;
	msg.read(_mediapipe_stream);

	{
		char ok = 1;
		_mediapipe_stream.Send(&ok, sizeof(ok)); 
	}

	for (uint32_t i = 0; i < msg.getHeader().nb_skeletons; i++) {
		uint32_t skeleton_i = msg.getSecondaryHeaders()[i].info_int[0];

		for (uint32_t j = 0; j < msg.getSecondaryHeaders()[i].nb_points; j++) {
			float x = msg.getPoints()[i][j].x * 12;
			float y = -msg.getPoints()[i][j].y * 12;
			float z = msg.getPoints()[i][j].z;
			_hands[skeleton_i][j].setPositionAbsolute(x, y, z);
		}

		glm::vec3 pos = _hands[skeleton_i][0].getPositionAbsolute();

		pos.x = msg.getPoints()[i][0].info[0] * 5 - 3;
		pos.y = -msg.getPoints()[i][0].info[1] * 2 + 0.5f;
		pos.z = msg.getPoints()[i][0].info[2] * 3;

		//pos.z = 0.5 * (_wrist_pos[skeleton_i].z + pos.z);
		// pos.z = pos.z * 64;
		_hands[skeleton_i][0].setPositionAbsoluteUpdateChildren(pos);

		/*if ((_frame_i % 2) == 0) {
			pos.z = _hands[skeleton_i][0].getPositionAbsolute().z;
			pos.z = pos.z * 64;
			_wrist_pos[skeleton_i] = pos;
		}
		_hands[skeleton_i][0].setPositionAbsoluteUpdateChildren(_wrist_pos[skeleton_i]);
	*/}

	_frame_i++;
}

void MediapipeModule::close() {
	Module::close();

	_mediapipe_stream.Close();
	_server_socket.Close();
}

std::vector<glm::vec3> MediapipeModule::cube() {
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

std::vector<uint32_t> MediapipeModule::cubeIndex() {
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