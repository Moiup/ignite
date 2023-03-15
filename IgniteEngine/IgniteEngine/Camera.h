#pragma once
#include "Entity3D.h"

class Camera : public Entity3D
{
private:
	glm::vec3 _eye;
	glm::vec3 _center;
	glm::vec3 _up;

	glm::mat4 _clip;

public:
	Camera();

	void setEye(glm::vec3 eye);
	void setCenter(glm::vec3 center);
	void setUp(glm::vec3 up);

	glm::mat4 getView();
	virtual glm::mat4 getProjection();
	glm::mat4 getClip();

	glm::mat4 getMVP();
};

