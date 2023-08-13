#pragma once
#include <glm/glm.hpp>
#include "fast_obj/fast_obj.h"

struct Material
{
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	glm::vec3 Ke;
	glm::vec3 Kt;
	float Ns;
	float Ni;
	glm::vec3 Tf;
	float d;
	int illum;

	Material();
	Material(fastObjMaterial& mat);

	const void setMaterial(fastObjMaterial& mat);
};

