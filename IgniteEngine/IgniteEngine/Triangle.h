#pragma once

#include "Object3D.h"
#include "Hittable.h"

#include <iostream>

class Triangle;

struct RTScene {
	std::vector<Triangle> _triangles;
	std::vector<Material> _materials;

	std::string string();
};

class Triangle : public Hittable
{
private:
	glm::vec3 _A;
	glm::vec3 _B;
	glm::vec3 _C;
	uint32_t _mat_id;

	glm::vec3 _n;

public:
	Triangle();
	Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C);
	Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, uint32_t mat_id);

	Hit intersectSimple(const Ray& ray);
	Hit intersect(const Ray& ray);

	const glm::vec3& A() const;
	const glm::vec3& B() const;
	const glm::vec3& C() const;
	const uint32_t mat_id() const;

	static void buildTriangles(glm::mat4 model, RTScene& scene);
	//static std::vector<Triangle> buildTriangles(Renderer* renderer, GraphicShader* graphic_shader);
};

