#include "Ray.h"

Ray::Ray() :
	_o{},
	_d{}
{
	;
}

Ray::Ray(glm::vec3 o, glm::vec3 e) :
	_o{ o },
	_d{ e - o }
{
	;
}

Ray::Ray(int32_t x, int32_t y, glm::mat4 inv_mvpv) {
	glm::vec4 o = inv_mvpv * glm::vec4(
		static_cast<float>(x) + 0.5f,
		static_cast<float>(y) + 0.5f,
		0,
		1.0f
	);

	glm::vec4 e = inv_mvpv * glm::vec4(
		static_cast<float>(x) + 0.5f,
		static_cast<float>(y) + 0.5f,
		1.0f,
		1.0f
	);

	o /= o.w;
	e /= e.w;

	_o = glm::vec3(o);
	_d = glm::vec3(e) - _o;
}

const glm::vec3& Ray::o() const {
	return _o;
}

const glm::vec3& Ray::d() const {
	return _d;
}

glm::vec3 Ray::p(float t) const{
	return _o + t * _d;
}