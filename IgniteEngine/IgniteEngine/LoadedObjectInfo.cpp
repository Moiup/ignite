#include "LoadedObjectInfo.h"

LoadedObjectInfo::LoadedObjectInfo() { ; }

void LoadedObjectInfo::loadWavefont(const std::string& file_name) {
	fastObjMesh* fom = fast_obj_read(file_name.c_str());
	if (!fom) {
		std::cout << "Error opening the file: " << file_name << std::endl;
		throw std::runtime_error("Error opening file.");
	}

	std::cout << "Loading obj file: ";
	std::cout << file_name << std::endl;

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
	std::vector<uint32_t> mat_id{};
	std::vector<glm::vec3> v_coord{};
	std::vector<glm::vec3> n_coord{};
	std::vector<glm::vec2> t_coord{};

	std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>> is_vertex;

	{
		uint32_t f = 0;
		uint32_t v_i = 0;
		while (f < fom->face_count) {
			buildFace(fom, indices, mat_id, v_coord, n_coord, t_coord, is_vertex, f, v_i);
			buildFace(fom, indices, mat_id, v_coord, n_coord, t_coord, is_vertex, f, v_i + 1);
			buildFace(fom, indices, mat_id, v_coord, n_coord, t_coord, is_vertex, f, v_i + 2);

			if (fom->face_vertices[f] == 4) {
				buildFace(fom, indices, mat_id, v_coord, n_coord, t_coord, is_vertex, f, v_i);
				buildFace(fom, indices, mat_id, v_coord, n_coord, t_coord, is_vertex, f, v_i + 2);
				buildFace(fom, indices, mat_id, v_coord, n_coord, t_coord, is_vertex, f, v_i + 3);
			}

			//else {
			//	// Here it is 4
			//	// -- First triangle
			//	indices.push_back(fom->indices[v_i].p);
			//	indices.push_back(fom->indices[v_i + 1].p);
			//	indices.push_back(fom->indices[v_i + 2].p);
			//	// -- Second triangle
			//	indices.push_back(fom->indices[v_i].p);
			//	indices.push_back(fom->indices[v_i + 2].p);
			//	indices.push_back(fom->indices[v_i + 3].p);
			//}

			v_i += fom->face_vertices[f];
			f++;
		}
		std::cout << v_i << std::endl;
	}

	std::cout << fom->index_count << " " << fom->face_count * 4 << " " << indices.size() << " " << v_coord.size() << " " << n_coord.size() << " " << t_coord.size() << std::endl;

	_meshes.push_back(Mesh());
	_meshes[0].setCoords(&v_coord[0][0], v_coord.size());
	_meshes[0].setNormals(&n_coord[0][0], n_coord.size());
	_meshes[0].setUV(&t_coord[0][0], t_coord.size());
	_meshes[0].setIndices(indices.data(), indices.size());

	std::cout << _meshes[0].getCoords().size() << " " << _meshes[0].getNormals().size() << " " << _meshes[0].getUV().size() << std::endl;

	_material_indices.push_back(std::vector<uint32_t>());
	_material_indices[0].assign(mat_id.data(), mat_id.data() + mat_id.size());
	
	fast_obj_destroy(fom);
	std::cout << " loaded!" << std::endl;
}

void LoadedObjectInfo::buildFace(
	fastObjMesh* fom,
	std::vector<uint32_t>& indices,
	std::vector<uint32_t>& mat_id,
	std::vector<glm::vec3>& v_coord,
	std::vector<glm::vec3>& n_coord,
	std::vector<glm::vec2>& t_coord,
	std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>>& is_vertex,
	uint32_t f,
	uint32_t v_i
) {
	uint32_t p_i = fom->indices[v_i].p;
	uint32_t n_i = fom->indices[v_i].n;
	uint32_t t_i = fom->indices[v_i].t;

	if (!is_vertex.count(p_i)) {
		is_vertex[p_i][n_i][t_i] = v_coord.size();
		v_coord.push_back({
			fom->positions[3 * p_i],
			fom->positions[3 * p_i + 1],
			fom->positions[3 * p_i + 2]
			});
		n_coord.push_back({
			fom->normals[3 * n_i],
			fom->normals[3 * n_i + 1],
			fom->normals[3 * n_i + 2]
			});
		t_coord.push_back({
			fom->texcoords[2 * t_i],
			fom->texcoords[2 * t_i + 1]
			});
		mat_id.push_back(fom->face_materials[f]);
	}
	else if (!is_vertex[p_i].count(n_i)) {
		is_vertex[p_i][n_i][t_i] = v_coord.size();
		v_coord.push_back({
			fom->positions[3 * p_i],
			fom->positions[3 * p_i + 1],
			fom->positions[3 * p_i + 2]
			});
		n_coord.push_back({
			fom->normals[3 * n_i],
			fom->normals[3 * n_i + 1],
			fom->normals[3 * n_i + 2]
			});
		t_coord.push_back({
			fom->texcoords[2 * t_i],
			fom->texcoords[2 * t_i + 1]
			});
		mat_id.push_back(fom->face_materials[f]);
	}
	else if (!is_vertex[p_i][n_i].count(t_i)) {
		is_vertex[p_i][n_i][t_i] = v_coord.size();
		v_coord.push_back({
			fom->positions[3 * p_i],
			fom->positions[3 * p_i + 1],
			fom->positions[3 * p_i + 2]
			});
		n_coord.push_back({
			fom->normals[3 * n_i],
			fom->normals[3 * n_i + 1],
			fom->normals[3 * n_i + 2]
			});
		t_coord.push_back({
			fom->texcoords[2 * t_i],
			fom->texcoords[2 * t_i + 1]
			});
		mat_id.push_back(fom->face_materials[f]);
	}
	indices.push_back(is_vertex[p_i][n_i][t_i]);
}