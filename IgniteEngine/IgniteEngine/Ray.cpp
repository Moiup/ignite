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

const glm::vec3& Ray::o() const {
	return _o;
}

const glm::vec3& Ray::d() const {
	return _d;
}

glm::vec3 Ray::p(float t) const{
	return _o + t * _d;
}