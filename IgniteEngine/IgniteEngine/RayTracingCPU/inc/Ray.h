#pragma once

#include "glm/vec3.hpp"

class Ray
{
private:
	glm::vec3 _o;
	glm::vec3 _d;

public:
	Ray();
	Ray(glm::vec3 o, glm::vec3 e);

	const glm::vec3& o() const;

	const glm::vec3& d() const;

	glm::vec3 p(float t) const;
};

