#pragma once
#include "glm/vec3.hpp"
#include <limits>

class Hit
{
private:
	float _t;

public:
	Hit();
	Hit(float t);

	const float t() const;
	float& t();

	operator bool() const;

	bool operator>(Hit& h);
	bool operator<(Hit& h);

	static const float inf();
};

