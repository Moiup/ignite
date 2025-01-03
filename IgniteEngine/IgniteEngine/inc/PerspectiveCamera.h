#pragma once
#include "DefaultConf.h"
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

	Entity3D* clone() const;
	PerspectiveCamera& operator=(const PerspectiveCamera& pc);
	void copyAttributes(const PerspectiveCamera& pc);

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

