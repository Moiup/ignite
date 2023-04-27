#pragma once

#include "Camera.h"
#include "PerspectiveCamera.h"

class CameraScene : public EngineEntity
{
private:
	PerspectiveCamera _perspective_camera;

	bool _was_middle_clicked;
	bool _is_middle_clicked;
	glm::vec2 _middle_prev_mouse;
	
	bool _was_right_clicked;
	bool _is_right_clicked;
	glm::vec2 _right_prev_mouse;

public:
	CameraScene();

	void start();
	void update();

	PerspectiveCamera& getPerspectiveCamera();
};

