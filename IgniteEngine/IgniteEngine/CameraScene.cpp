#include "CameraScene.h"

CameraScene::CameraScene() :
	_perspective_camera{},
	_was_middle_clicked{false},
	_is_middle_clicked{false},
	_middle_prev_mouse{},
	_right_prev_mouse{}
{
}

void CameraScene::start() {
	EngineEntity::start();

	_perspective_camera.setAspectRatio(
		static_cast<float>(DefaultConf::render_window->getWidth()) /
		static_cast<float>(DefaultConf::render_window->getHeight())
	);

	_perspective_camera.setFOVY(glm::radians(45.0f));
	_perspective_camera.setNear(0.1f);
	_perspective_camera.setFar(100.0f);
	_perspective_camera.setEye(glm::vec3(0, 0, -1));
	_perspective_camera.setCenter(glm::vec3(0, 0, 0));
}

void CameraScene::update() {
	EngineEntity::update();

	SDL_Event* event = DefaultConf::event;
	glm::vec3 cam_pos = _perspective_camera.getPositionAbsolute();
	
	if (event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
		_is_middle_clicked = (event->button.button == SDL_BUTTON_MIDDLE) && (event->button.state == SDL_PRESSED);
		_is_right_clicked = (event->button.button == SDL_BUTTON_RIGHT) && (event->button.state == SDL_PRESSED);
	}

	// Translating the camera verticaly and horizontaly
	if (_is_middle_clicked) {
		if (!_was_middle_clicked) {
			_middle_prev_mouse.x = event->button.x;
			_middle_prev_mouse.y = event->button.y;
			_was_middle_clicked = true;
		}
		if (event->button.x != 0 && event->button.y != 0) {
			glm::vec3 dir = glm::vec3(
				event->button.x - _middle_prev_mouse.x,
				event->button.y - _middle_prev_mouse.y,
				0
			);

			if (dir.x != 0 && dir.y != 0) {
				dir = glm::normalize(dir);
				//dir = _perspective_camera.getRotate() * glm::vec4(dir, 1);

				_perspective_camera.setPositionAbsolute(
					cam_pos + dir * 0.1f
				);
			}
		}
	}
	else {
		_was_middle_clicked = false;
	}

	// Rotating the camera
	if (_is_right_clicked) {
		if (!_was_right_clicked) {
			_right_prev_mouse.x = event->button.x;
			_right_prev_mouse.y = event->button.y;
			_was_right_clicked = true;
		}

		if (event->button.x != 0 && event->button.y != 0) {
			glm::vec3 dir = glm::vec3(
				_right_prev_mouse.x - event->button.x,
				_right_prev_mouse.y - event->button.y,
				0
			);

			glm::vec3 cam_rot = _perspective_camera.getRotationAbsolute();

			if (dir.x != 0 && dir.y != 0) {
				dir = glm::normalize(dir);
				_perspective_camera.setRotationAbsolute(cam_rot.x - dir.y * 0.05, cam_rot.y + dir.x * 0.05, 0);
			}
		}
	}
	else {
		_was_right_clicked = false;
	}

	// Translating camera forward and backward with mouse wheel
	if (event->type == SDL_MOUSEWHEEL) {
		glm::vec3 eye = _perspective_camera.getEye();
		glm::vec3 center = _perspective_camera.getCenter();
		glm::vec3 dir = glm::normalize(center - eye);

		if (event->wheel.y > 0) {
			_perspective_camera.setPositionAbsolute(cam_pos - dir);
		}
		else if (event->wheel.y < 0) {
			_perspective_camera.setPositionAbsolute(cam_pos + dir);
		}
	}
}

PerspectiveCamera& CameraScene::getPerspectiveCamera() {
	return _perspective_camera;
}