#include "LoadedObjectInfo.h"

LoadedObjectInfo::LoadedObjectInfo() { ; }

void LoadedObjectInfo::loadWavefont(const std::string& file_name) {
	fastObjMesh* fom = fast_obj_read(file_name.c_str());
	if (!fom) {
		std::cout << "Error opening the file: " << file_name << std::endl;
		throw std::runtime_error("Error opening file.");
	}

	std::cout << file_name << std::endl;
	std::cout << fom->object_count << std::endl;

	_materials.push_back(std::vector<Material>());
	std::unordered_map<char*, uint32_t> mat_to_tex;
	uint32_t t_id{0};
	for (uint32_t i = 0; i < fom->material_count; i++) {
		_materials[0].push_back(Material(fom->materials[i]));

		if (!fom->materials[i].map_Kd.path) {
			continue;
		}

		if (!mat_to_tex.count(fom->materials[i].map_Kd.path)) {
			mat_to_tex[fom->materials[i].map_Kd.path] = t_id;
			t_id++;
		}
		std::cout << mat_to_tex[fom->materials[i].map_Kd.path] << std::endl;
		_materials[0][i].map_Kd = mat_to_tex[fom->materials[i].map_Kd.path];

		std::cout << fom->materials[i].map_Kd.path << std::endl;
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

	//_mercedes_tex.readFile("../assets/textures/mercedes.png");
	//_mercedes_tex.setLogicalDevice(DefaultConf::logical_device);
	//_mercedes_tex.setGPU(DefaultConf::gpu);
	//_mercedes_tex.setCommandPool(DefaultConf::command_pool);
	//_mercedes_tex.create();

	_meshes.push_back(Mesh());
	_meshes[0].setCoords(fom->positions, fom->position_count);
	_meshes[0].setNormals(fom->normals, fom->normal_count);
	_meshes[0].setUV(fom->texcoords, fom->texcoord_count);
	_meshes[0].setIndices(indices.data(), indices.size());

	_material_indices.push_back(std::vector<uint32_t>());
	_material_indices[0].assign(mat_id.data(), mat_id.data() + mat_id.size());
	
	fast_obj_destroy(fom);
}

//void LoadedObjectInfo::loadWavefont(const std::string& file_name) {
//	fastObjMesh* fom = fast_obj_read(file_name.c_str());
//	if (!fom) {
//		std::cout << "Error opening the file: " << file_name << std::endl;
//		throw std::runtime_error("Error opening file.");
//	}
//
//	std::cout << file_name << std::endl;
//	std::cout << fom->object_count << std::endl;
//
//	std::cout << fom->position_count << std::endl;
//	std::cout << fom->face_count << std::endl;
//
//	exit(0);
//
//	_meshes.resize(fom->object_count);
//
//	std::cout << "TEST 1" << std::endl;
//	// For each object
//	for (uint32_t o_i = 0; o_i < fom->object_count; o_i++) {
//		std::vector<uint32_t> indices{};
//		std::vector<float> positions{};
//		std::vector<float> normals{};
//		std::vector<float> uv{};
//		std::cout << "TEST 1.1" << std::endl;
//		fastObjGroup fog = fom->objects[o_i];
//		uint32_t v_i = fog.index_offset;
//		// For each face of the object
//		for (uint32_t f_i = fog.face_offset; f_i < fog.face_offset + fog.face_count; f_i++) {
//			if (fom->face_vertices[f_i] == 3) {
//				indices.push_back(fom->indices[v_i].p - fog.index_offset - 1);
//				indices.push_back(fom->indices[v_i + 1].p - fog.index_offset - 1);
//				indices.push_back(fom->indices[v_i + 2].p - fog.index_offset - 1);
//			}
//			else {
//				// Here it is 4
//				// -- First triangle
//				indices.push_back(fom->indices[v_i].p - fog.index_offset - 1);
//				indices.push_back(fom->indices[v_i + 1].p - fog.index_offset - 1);
//				indices.push_back(fom->indices[v_i + 2].p - fog.index_offset - 1);
//				// -- Second triangle
//				indices.push_back(fom->indices[v_i].p - fog.index_offset - 1);
//				indices.push_back(fom->indices[v_i + 2].p - fog.index_offset - 1);
//				indices.push_back(fom->indices[v_i + 3].p - fog.index_offset - 1);
//			}
//			v_i += fom->face_vertices[f_i];
//		}
//		std::cout << "TEST 1.2" << std::endl;
//		_meshes[o_i].setIndices(indices.data(), indices.size());
//		std::cout << "TEST 1.3" << std::endl;
//		std::cout << fom->position_count << " " << fom->face_count << std::endl;
//		std::cout << fog.face_offset * fom->face_vertices[0] << " " << fog.face_count * fom->face_vertices[0] << std::endl;
//		_meshes[o_i].setCoords(
//			fom->positions + (fog.face_offset * fom->face_vertices[0]),
//			fog.face_count * fom->face_vertices[0]
//		);
//		std::cout << "TEST 1.4" << std::endl;
//	}
//	std::cout << "TEST 2" << std::endl;
//
//	fast_obj_destroy(fom);
//}

//void LoadedObjectInfo::loadWavefont(const std::string& file_name) {
//	std::ifstream file;
//
//	file.open(file_name);
//	if (!file.is_open()) {
//		std::cout << "Error opening the file: " << file_name << std::endl;
//		throw std::runtime_error("Error opening file.");
//	}
//
//	//std::string line;
//	while (!file.eof()) {
//		std::string head;
//		file >> head;
//		
//		//if (head == "mtllib") {
//		//	// Do something
//		//	continue;
//		//}
//		if (head == "usemtl") {
//			std::cout << "object" << std::endl;
//			//_meshes.resize(_meshes.size() + 1);
//		}
//		else if (head == "f") {
//
//		}
//		else if (head == "v") {
//
//		}
//		else if (head == "vn") {
//
//		}
//		else if (head == "vt") {
//
//		}
//	}
//
//	file.close();
//}