#include "LoadedObjectInfo.h"

LoadedObjectInfo::LoadedObjectInfo() { ; }

void LoadedObjectInfo::loadObj(const std::string& file_name) {
	fastObjMesh* fom = fast_obj_read(file_name.c_str());
	if (!fom) {
		std::cout << "Error opening the file: " << file_name << std::endl;
		throw std::runtime_error("Error opening file.");
	}

	std::vector<uint32_t> indices{};
	std::vector<uint32_t> mat_id(fom->position_count);
	std::vector<bool>exists(fom->position_count, false);
	{
		uint32_t f = 0;
		uint32_t v_i = 0;
		while (f < fom->face_count) {
			if (fom->face_vertices[f] == 3) {
				indices.push_back(fom->indices[v_i].p);
				indices.push_back(fom->indices[v_i + 1].p);
				indices.push_back(fom->indices[v_i + 2].p);
			}
			else {
				// Here it is 4
				// -- First triangle
				indices.push_back(fom->indices[v_i ].p);
				indices.push_back(fom->indices[v_i + 1].p);
				indices.push_back(fom->indices[v_i + 2].p);
				// -- Second triangle
				indices.push_back(fom->indices[v_i].p);
				indices.push_back(fom->indices[v_i + 2].p);
				indices.push_back(fom->indices[v_i + 3].p);
			}

			for (uint32_t i = v_i; i < v_i + fom->face_vertices[f]; i++) {
				uint32_t ind = fom->indices[i].p;
				if (exists[ind]) {
					continue;
				}
				mat_id[ind] = fom->face_materials[f];
				exists[ind] = true;
			}
			v_i += fom->face_vertices[f];
			f++;
		}
	}

	_mesh.setCoords(fom->positions, fom->position_count);
	_mesh.setNormals(fom->normals, fom->normal_count);
	_mesh.setUV(fom->texcoords, fom->texcoord_count);
	_mesh.setIndices(indices.data(), indices.size());
	_mesh.setIndicesToMaterial(mat_id.data(), mat_id.size());
	_mesh.setMaterials(fom->materials, fom->material_count);

	fast_obj_destroy(fom);
}