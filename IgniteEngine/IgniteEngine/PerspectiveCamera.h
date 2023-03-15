#pragma once
#include "Camera.h"

class PerspectiveCamera : public Camera
{
private:
	float _fovy;
	float _aspect_ratio;
	float _near;
	float _far;

public:
	PerspectiveCamera();

	glm::mat4 getProjection();

	void setFOVY(float fovy);
	void setAspectRatio(float aspect_ratio);
	void setNear(float near);
	void setFar(float far);

	float getFOVY();
	float getAspectRatio();
	float getNear();
	float getFar();
};

