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

Entity3D* PerspectiveCamera::clone() const {
	PerspectiveCamera* pc = new PerspectiveCamera;

	pc->copyAttributes(*this);

	for (Entity3D* child : this->_children) {
		PerspectiveCamera* child_pc = reinterpret_cast<PerspectiveCamera*>(child);
		pc->addChild(child_pc->clone());
	}

	return pc;
}

PerspectiveCamera& PerspectiveCamera::operator=(const PerspectiveCamera& pc) {
	copyAttributes(pc);

	for (Entity3D* child : pc._children) {
		PerspectiveCamera* child_pc = reinterpret_cast<PerspectiveCamera*>(child);
		addChild(child_pc->clone());
	}

	return *this;
}

void PerspectiveCamera::copyAttributes(const PerspectiveCamera& pc) {
	_fovy = pc._fovy;
	_aspect_ratio = pc._aspect_ratio;
	_near = pc._near;
	_far = pc._far;
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