#pragma once

#include "Hit.h"
#include "Ray.h"

class Hittable
{
public:
	Hittable();

	virtual Hit intersect(const Ray& ray) = 0;
};

