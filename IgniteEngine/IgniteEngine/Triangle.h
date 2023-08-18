#pragma once

#include "Object3D.h"
#include "Hittable.h"

class Triangle : public Hittable
{
private:
	glm::vec3 _A;
	glm::vec3 _B;
	glm::vec3 _C;
	uint32_t _mat_id;

public:
	Triangle();
	Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C);
	Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, uint32_t mat_id);

	Hit intersect(const Ray& ray);

	const glm::vec3& A() const;
	const glm::vec3& B() const;
	const glm::vec3& C() const;
	const uint32_t mat_id() const;

	static std::pair<std::vector<Triangle>, std::vector<Material>> buildTriangles();
	//static std::vector<Triangle> buildTriangles(Renderer* renderer, GraphicShader* graphic_shader);
};

