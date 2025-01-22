#pragma once

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Ray
{
private:
	glm::vec3 _o;
	glm::vec3 _d;

public:
	Ray();
	Ray(glm::vec3 o, glm::vec3 e);
	Ray(int32_t x, int32_t y, glm::mat4 inv_mvpv);

	const glm::vec3& o() const;

	const glm::vec3& d() const;

	glm::vec3 p(float t) const;
};

