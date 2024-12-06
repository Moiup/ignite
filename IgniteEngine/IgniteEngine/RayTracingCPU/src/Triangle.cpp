#include "Triangle.h"

void RTScene::clear() {
	_triangles.clear();
	_materials.clear();
}

std::string RTScene::string() {
	std::string str = "RTScene:\n";
	str += "\ttriangles: " + std::to_string(_triangles.size()) + "\n";
	str += "\tmaterials: " + std::to_string(_materials.size()) + "\n";

	return str;
}

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
	_AB{_B - _A},
	_AC{_C - _A},
	_mat_id{ mat_id },
	_n{}
{
	_n = glm::normalize(glm::cross(_AB, _AC));
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
	glm::vec3 p_vec = glm::cross(ray.d(), _AC);
	float det = glm::dot(_AB, p_vec);
	float inv_det = 1.0f / det;

	glm::vec3 t_vec = ray.o() - _A;

	float u = glm::dot(t_vec, p_vec) * inv_det;
	if (u < 0 || u > 1)
		return Hit();

	glm::vec3 q_vec = glm::cross(t_vec, _AB);
	float v = glm::dot(ray.d(), q_vec) * inv_det;
	if (v < 0 || u + v > 1)
		return Hit();

	float t = glm::dot(_AC, q_vec) * inv_det;
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


void Triangle::buildTriangles(RTScene& scene) {
	//materials.push_back(Material());

	const std::unordered_map<Renderer*, std::unordered_map<GraphicsPipeline*, Object3DArrays>>& arrays = Object3D::getArrays();



	std::unordered_map<Material*, uint32_t> is_mat{};

	uint32_t nb_mat{ 0 };

	// For each renderer
	for (auto& r_gp : arrays) {
		Renderer* renderer = r_gp.first;
		if (!renderer) {
			continue;
		}

		auto& gp_arr = r_gp.second;
		// For each graphic pipeline
		for (auto& gp_mesh : gp_arr) {
			GraphicsPipeline* gp = gp_mesh.first;
			if (!gp) {
				continue;
			}

			auto& mesh_arr = gp_mesh.second.mesh_objects;
			// For each mesh
			for (auto& m_o : mesh_arr) {
				Mesh* mesh = m_o.first;
				const std::vector<glm::vec3>& coords = mesh->getCoords();
				const std::vector<uint32_t>& indices = mesh->getIndices();
				
				//const std::vector<uint32_t>& mat_indices = mesh->getIndicesToMaterial();
				//const std::vector<Material>& mesh_materials = mesh->getMaterials();
				
				const std::vector<Object3D*>& objects = m_o.second;
				
				// For each objects of the same mesh
				if (!objects.size()) {
					continue;
				}
				for (Object3D* obj : objects) {
					//glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0, 10.0));
					glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
					
					glm::mat4 tr = obj->getTransform();
					std::vector<Material*> materials = obj->getMaterial();
					std::vector<uint32_t>* mat_indices = obj->getMaterialIndices();

					std::cout << indices.size() << std::endl;
					std::cout << mat_indices->size() << std::endl;
					std::cout << coords.size() << std::endl;
					
					for (uint32_t ind = 0; ind < indices.size(); ind += 3) {
						glm::vec3 A = tr * glm::vec4(coords[indices[ind]], 1.0);
						glm::vec3 B = tr * glm::vec4(coords[indices[ind + 1]], 1.0);
						glm::vec3 C = tr * glm::vec4(coords[indices[ind + 2]], 1.0);

						Material* mat = materials[(*mat_indices)[indices[ind]]];
						if (!is_mat.count(mat)) {
							is_mat[mat] = nb_mat;
							scene._materials.push_back(*mat);
							nb_mat++;
						}

						uint32_t mat_id = is_mat[mat];

						scene._triangles.push_back(
							Triangle(A, B, C, mat_id)
						);
					}
				}
			}
		}
	}
}

