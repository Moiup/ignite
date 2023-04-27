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
	void setEye(float x, float y, float z);
	void setCenter(glm::vec3 center);
	void setCenter(float x, float y, float z);
	void setUp(glm::vec3 up);
	void setUp(float x, float y, float z);

	glm::vec3 getEye();
	glm::vec3 getCenter();
	glm::vec3 getUp();

	glm::mat4 getView();
	virtual glm::mat4 getProjection();
	glm::mat4 getClip();

	glm::mat4 getMVP();
};

