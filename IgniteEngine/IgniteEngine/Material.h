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

	int map_Ka;
	int map_Kd;
	int map_Ks;
	int map_Ke;
	int map_Kt;
	int map_Ns;
	int map_Ni;
	int map_d;
	int map_bump;

	Material();
	Material(fastObjMaterial& mat);

	const void setMaterial(fastObjMaterial& mat);
};

namespace glsl {
	struct Mat
	{
		glm::vec3 Ka;
		int map_Ka;
		
		glm::vec3 Kd;
		int map_Kd;
		
		glm::vec3 Ks;
		int map_Ks;
		
		glm::vec3 Ke;
		int map_Ke;
		
		glm::vec3 Kt;
		int map_Kt;
		
		glm::vec3 Tf;
		float pad0;

		float Ns;
		float Ni;
		float d;
		float illum;

		int map_Ns;
		int map_Ni;
		int map_d;
		int map_bump;

		Mat();
		Mat(Material mat);

		const void setMaterial(Material mat);
	};
}