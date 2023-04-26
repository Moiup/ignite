#pragma once

#include "Camera.h"
#include "PerspectiveCamera.h"

class CameraScene : public EngineEntity
{
private:
	PerspectiveCamera _perspective_camera;

public:
	CameraScene();

	void start();
	void update();

	PerspectiveCamera& getPerspectiveCamera();
};

