#pragma once
#include "EngineEntity.h"
#include "Module.h"
#include "Object3D.h"
#include "Renderer.h"

#include "ESocket/EServerSocket.h"
#include "InitNetworkProtocole.h"
#include "MocapMessageProtocolNetwork.h"

#include <string>

class MediapipeModule : public Module
{
public:
	const std::string SERVER_ADDRESS{ "127.0.0.1" };
	const std::string SERVER_PORT{ "8080" };

private:
	EServerSocket _server_socket;
	EStream _mediapipe_stream;

	Mesh _cube;
	std::vector<std::vector<Object3D>> _hands;

	uint32_t _frame_i;
	std::vector<glm::vec3> _wrist_pos;

public:
	MediapipeModule();

	void init();
	void start();
	void update();
	void close();

private:

	std::vector<glm::vec3> cube();
	std::vector<uint32_t> cubeIndex();
};

