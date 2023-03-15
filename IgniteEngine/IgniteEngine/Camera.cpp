#include "Camera.h"

Camera::Camera() :
	Entity3D::Entity3D(),
	_eye{ 0, 0, 1.0f },
	_center{ 0, 0, 0 },
	_up{ 0, 1, 0 }
{
	_clip = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f
	);
}

void Camera::setEye(glm::vec3 eye) {
	_eye = eye;
}

void Camera::setCenter(glm::vec3 center) {
	_center = center;
}

void Camera::setUp(glm::vec3 up) {
	_up = up;
}

glm::mat4 Camera::getView() {
	glm::mat4 view = glm::lookAt(
		_eye,
		_center,
		_up
	);

	return view;
}

glm::mat4 Camera::getProjection() {
	glm::mat4 projection = glm::mat4(1.0);
	return projection;
}

glm::mat4 Camera::getClip() {
	return _clip;
}

glm::mat4 Camera::getMVP() {
	glm::mat4 mvp = getClip() * getProjection() * getView() * getTransform();
	return mvp;
}