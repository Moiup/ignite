#include "PerspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera() :
	Camera::Camera(),
	_fovy{glm::radians(45.0f)},
	_aspect_ratio{16/9},
	_near{0.1f},
	_far{100.0f}
{
	;
}

glm::mat4 PerspectiveCamera::getProjection() {
	glm::mat4 p = glm::perspective(
		_fovy,
		_aspect_ratio,
		_near,
		_far
	);
	return p;
}

void PerspectiveCamera::setFOVY(float fovy) {
	_fovy = fovy;
}

void PerspectiveCamera::setAspectRatio(float aspect_ratio) {
	_aspect_ratio = aspect_ratio;
}

void PerspectiveCamera::setNear(float near) {
	_near = near;
}

void PerspectiveCamera::setFar(float far) {
	_far = far;
}

float PerspectiveCamera::getFOVY() {
	return _fovy;
}

float PerspectiveCamera::getAspectRatio() {
	return _aspect_ratio;
}

float PerspectiveCamera::getNear() {
	return _near;
}

float PerspectiveCamera::getFar() {
	return _far;
}