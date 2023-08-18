#include "Triangle.h"

Triangle::Triangle() :
	Hittable::Hittable(),
	_A{},
	_B{},
	_C{},
	_mat_id{ 0 }
{
	;
}

Triangle::Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C) :
	Hittable::Hittable(),
	_A{ A },
	_B{ B },
	_C{ C },
	_mat_id{ 0 }
{
	;
}

Triangle::Triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C, uint32_t mat_id) :
	Hittable::Hittable(),
	_A{ A },
	_B{ B },
	_C{ C },
	_mat_id{ mat_id }
{
	;
}

Hit Triangle::intersect(const Ray& ray) {
	return Hit();
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


std::pair<std::vector<Triangle>, std::vector<Material>> Triangle::buildTriangles() {
	std::vector<Triangle> triangles;
	std::vector<Material> materials{Material()};

	std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>>& mesh_objects = Object3D::getMeshObjects();

	// For each renderer
	for (auto& r_gs : mesh_objects) {
		auto& gs_arr = r_gs.second;
		// For each graphic shader
		for (auto& gs_mesh : gs_arr) {
			auto& mesh_arr = gs_mesh.second;
			// For each mesh
			for (auto& m_o : mesh_arr) {
				Mesh* mesh = m_o.first;
				const std::vector<glm::vec3>& coords = mesh->getCoords();
				const std::vector<uint32_t>& indices = mesh->getIndices();
				const std::vector<uint32_t>& mat_indices = mesh->getIndicesToMaterial();
				const std::vector<Material>& mesh_materials = mesh->getMaterials();

				std::vector<Object3D*>& objects = m_o.second;
				// For each objects of the same mesh
				for (Object3D* obj : objects) {
					glm::mat4 tr = obj->getTransform();

					for (uint32_t ind = 0; ind < indices.size(); ind += 3) {
						glm::vec3 A = tr * glm::vec4(coords[ind], 1.0);
						glm::vec3 B = tr * glm::vec4(coords[ind + 1], 1.0);
						glm::vec3 C = tr * glm::vec4(coords[ind + 2], 1.0);
						uint32_t mat_id = 0;
						if (mesh_materials.size()) {
							mat_id = mat_indices[ind] + materials.size();
						}

						triangles.push_back(
							Triangle(A, B, C, mat_id)
						);
					}
				}

				// Adding the materials to the materials array
				materials.insert(materials.end(), mesh_materials.cbegin(), mesh_materials.cend());
			}
		}
	}
	return /*std::pair<std::vector<Triangle>, std::vector<Material>>*/{triangles, materials};
}

