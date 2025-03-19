#pragma once
#include "Entity3D.h"

class Camera : public Entity3D
{
private:
	glm::vec3 _eye;
	glm::vec3 _center;
	glm::vec3 _up;

	uint32_t _width;
	uint32_t _height;

	glm::mat4 _clip;

public:
	Camera();

	virtual Entity3D* clone() const = 0;
	virtual Camera& operator=(const Camera& c) { return *this; };
	void copyAttributes(const Camera& c);

	void setEye(glm::vec3 eye);
	void setEye(float x, float y, float z);
	void setCenter(glm::vec3 center);
	void setCenter(float x, float y, float z);
	void setUp(glm::vec3 up);
	void setUp(float x, float y, float z);

	const glm::vec3& getEye() const;
	const glm::vec3& getCenter() const;
	const glm::vec3& getUp() const;

	glm::mat4 getView() const;
	virtual glm::mat4 getProjection() = 0;
	glm::mat4 getClip() const;

	glm::mat4 getMVP();
	glm::mat4 getMVPC();

	static glm::mat4 getViewport(uint32_t width, uint32_t height);
};

