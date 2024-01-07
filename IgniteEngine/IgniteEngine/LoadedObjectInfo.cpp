#include "LoadedObjectInfo.h"

LoadedObjectInfo::LoadedObjectInfo() { ; }

void LoadedObjectInfo::loadWavefont(const std::string& file_name) {
	fastObjMesh* fom = fast_obj_read(file_name.c_str());
	if (!fom) {
		std::cout << "Error opening the file: " << file_name << std::endl;
		throw std::runtime_error("Error opening file.");
	}

	std::cout << "Loading obj file: ";
	std::cout << file_name;

	_materials.push_back(std::vector<Material>());
	_textures.push_back(std::vector<Texture>());
	std::unordered_map<char*, uint32_t> mat_to_tex;
	uint32_t t_id{0};
	for (uint32_t i = 0; i < fom->material_count; i++) {
		_materials[0].push_back(Material(fom->materials[i]));
		if (!fom->materials[i].map_Kd.path) {
			continue;
		}

		std::cout << fom->materials[i].map_Kd.path << std::endl;
		if (!mat_to_tex.count(fom->materials[i].map_Kd.path)) {
			mat_to_tex[fom->materials[i].map_Kd.path] = t_id;
			_textures[0].push_back(Texture());
			_textures[0][t_id].readFile(fom->materials[i].map_Kd.path);
			_textures[0][t_id].setLogicalDevice(DefaultConf::logical_device);
			_textures[0][t_id].setGPU(DefaultConf::gpu);
			_textures[0][t_id].setCommandPool(DefaultConf::command_pool);
			_textures[0][t_id].create();
			t_id++;
		}
		_materials[0][i].map_Kd = mat_to_tex[fom->materials[i].map_Kd.path];
	}

	std::vector<uint32_t> indices{};
	std::vector<uint32_t> mat_id(fom->position_count);
	std::vector<uint32_t> tex_id(fom->position_count);
	std::vector<bool>exists(fom->position_count, false);
	{
		uint32_t f = 0;
		uint32_t v_i = 0;
		uint32_t m_i = 0;
		while (f < fom->face_count) {
			if (fom->face_vertices[f] == 3) {
				indices.push_back(fom->indices[v_i].p);
				indices.push_back(fom->indices[v_i + 1].p);
				indices.push_back(fom->indices[v_i + 2].p);
			}
			else {
				// Here it is 4
				// -- First triangle
				indices.push_back(fom->indices[v_i].p);
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
				tex_id[ind] = _materials[0][fom->face_materials[f]].map_Kd;
				exists[ind] = true;
			}
			v_i += fom->face_vertices[f];
			f++;
		}
	}

	std::cout << fom->face_count << " " << fom->face_count * 3 << " " << fom->position_count << " " << fom->normal_count << " " << fom->texcoord_count << std::endl;

	_meshes.push_back(Mesh());
	_meshes[0].setCoords(fom->positions, fom->position_count);
	_meshes[0].setNormals(fom->normals, fom->normal_count);
	_meshes[0].setUV(fom->texcoords, fom->texcoord_count);
	_meshes[0].setIndices(indices.data(), indices.size());

	_material_indices.push_back(std::vector<uint32_t>());
	_material_indices[0].assign(mat_id.data(), mat_id.data() + mat_id.size());
	
	fast_obj_destroy(fom);
	std::cout << " loaded!" << std::endl;
}
