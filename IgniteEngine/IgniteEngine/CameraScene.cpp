#include "CameraScene.h"

CameraScene::CameraScene() :
	_perspective_camera{}
{
}

void CameraScene::start() {
	_perspective_camera.setAspectRatio(
		static_cast<float>(DefaultConf::render_window->getWidth()) /
		static_cast<float>(DefaultConf::render_window->getHeight())
	);

	_perspective_camera.setFOVY(glm::radians(45.0f));
	_perspective_camera.setNear(0.1f);
	_perspective_camera.setFar(100.0f);
	_perspective_camera.setEye(glm::vec3(0, 0, 5));
}

void CameraScene::update() {

}

PerspectiveCamera& CameraScene::getPerspectiveCamera() {
	return _perspective_camera;
}