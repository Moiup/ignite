#include "Triangle.h"

Triangle::Triangle() :
	Hittable::Hittable(),
	_A{},
	_B{},
	_C{},
	_mat_id{ 0 },
	_n{}
{
	;
}

Triangle::Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C) :
	Hittable::Hittable(),
	_A{ A },
	_B{ B },
	_C{ C },
	_mat_id{ 0 },
	_n{}
{
	;
}

Triangle::Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, uint32_t mat_id) :
	Hittable::Hittable(),
	_A{ A },
	_B{ B },
	_C{ C },
	_mat_id{ mat_id },
	_n{}
{
	_n = glm::normalize(glm::cross(_B - _A, _C - _A));
}

Hit Triangle::intersectSimple(const Ray& ray) {
	// Check if intersection between ray and plan exists
	float t = glm::dot(_n, _A - ray.o()) / glm::dot(_n, ray.d());

	if (t < 0.0f) {
		//std::cout << "NO NO" << std::endl;
		return Hit();
	}

	glm::vec3 p = ray.p(t);
	// Check if point is inside of the triangle
	if (glm::dot(_n, glm::cross(_B - _A, p - _A)) < 0) {
		return Hit();
	}
	if (glm::dot(_n, glm::cross(_C - _B, p - _B)) < 0) {
		return Hit();
	}
	if (glm::dot(_n, glm::cross(_A - _C, p - _C)) < 0) {
		return Hit();
	}

	return Hit(t);
}

Hit Triangle::intersect(const Ray& ray) {
	glm::vec3 ab = _B - _A;
	glm::vec3 ac = _C - _A;

	glm::vec3 p_vec = glm::cross(ray.d(), ac);
	float det = glm::dot(ab, p_vec);
	float inv_det = 1 / det;

	glm::vec3 t_vec = ray.o() - _A;

	float u = glm::dot(t_vec, p_vec) * inv_det;
	if (u < 0 || u > 1)
		return Hit();

	glm::vec3 q_vec = glm::cross(t_vec, ab);
	float v = glm::dot(ray.d(), q_vec) * inv_det;
	if (v < 0 || u + v > 1)
		return Hit();

	float t = glm::dot(ac, q_vec) * inv_det;
	if (t < 0 /* || t > Hit::inf()*/)
		return Hit();

	return Hit(t);
}

const glm::vec3& Triangle::A() const {
	return _A;
}

const glm::vec3& Triangle::B() const {
	return _B;
}

const glm::vec3& Triangle::C() const {
	return _C;
}

const uint32_t Triangle::mat_id() const {
	return _mat_id;
}


std::pair<std::vector<Triangle>, std::vector<Material>> Triangle::buildTriangles(glm::mat4 model) {
	std::vector<Triangle> triangles{};
	std::vector<Material> materials{};

	//materials.push_back(Material());

	std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>>& mesh_objects = Object3D::getMeshObjects();

	// For each renderer
	for (auto& r_gs : mesh_objects) {
		Renderer* renderer = r_gs.first;
		if (!renderer) {
			continue;
		}

		auto& gs_arr = r_gs.second;
		// For each graphic shader
		for (auto& gs_mesh : gs_arr) {
			GraphicShader* gs = gs_mesh.first;
			if (!gs) {
				continue;
			}

			auto& mesh_arr = gs_mesh.second;
			// For each mesh
			for (auto& m_o : mesh_arr) {
				Mesh* mesh = m_o.first;
				const std::vector<glm::vec3>& coords = mesh->getCoords();
				const std::vector<uint32_t>& indices = mesh->getIndices();
				//const std::vector<uint32_t>& mat_indices = mesh->getIndicesToMaterial();
				//const std::vector<Material>& mesh_materials = mesh->getMaterials();
				
				std::vector<Object3D*>& objects = m_o.second;
				
				// For each objects of the same mesh
				if (!objects.size()) {
					continue;
				}
				for (Object3D* obj : objects) {
					
					glm::mat4 tr = obj->getTransform();
					
					for (uint32_t ind = 0; ind < indices.size(); ind += 3) {
						glm::vec3 A = tr * glm::vec4(coords[indices[ind]], 1.0);
						glm::vec3 B = tr * glm::vec4(coords[indices[ind + 1]], 1.0);
						glm::vec3 C = tr * glm::vec4(coords[indices[ind + 2]], 1.0);
						uint32_t mat_id = 0;
						
						//if (mesh_materials.size()) {
						//	mat_id = mat_indices[indices[ind]] + materials.size();
						//}
						triangles.push_back(
							Triangle(A, B, C, 0)
						);
					}
				}
				//materials.insert(materials.end(), mesh_materials.begin(), mesh_materials.end());
			}
		}
	}

	return {triangles, materials};
}

