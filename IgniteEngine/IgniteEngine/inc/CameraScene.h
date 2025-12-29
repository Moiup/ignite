#pragma once

#include "Camera.h"
#include "PerspectiveCamera.h"

class CameraScene : public EngineEntity
{
private:
	PerspectiveCamera _perspective_camera{};
	Camera* _camera{nullptr};

	bool _was_middle_clicked{false};
	bool _is_middle_clicked{false};
	glm::vec2 _middle_prev_mouse{};
	
	bool _was_right_clicked{false};
	bool _is_right_clicked{false};
	glm::vec2 _right_prev_mouse{};
	glm::mat4 _mvp{};

public:
	CameraScene();

	void init();
	void start();
	void update();
	void close();

	void setCamera(Camera* camera);
	const Camera& camera() const;
	Camera& camera();
	glm::mat4 getProjection();
	glm::mat4 getMVPC();
	glm::mat4 getMVP();
};

