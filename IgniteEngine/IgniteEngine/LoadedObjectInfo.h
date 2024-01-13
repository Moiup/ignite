#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "DefaultConf.h"
#include "fast_obj/fast_obj.h"
#include <iostream>
#include <fstream>

struct LoadedObjectInfo
{
	std::vector<Mesh> _meshes;
	std::vector<std::vector<uint32_t>> _material_indices; // array of indices for each mesh
	std::vector<std::vector<Material>> _materials;
	std::vector<std::vector<Texture>> _textures;
	std::vector<LoadedObjectInfo> _children;

	LoadedObjectInfo();

	void loadWavefont(const std::string& file_name);

private:
	void buildFace(
		fastObjMesh* fom,
		std::vector<uint32_t>& indices,
		std::vector<uint32_t>& mat_id,
		std::vector<glm::vec3>& v_coord,
		std::vector<glm::vec3>& n_coord,
		std::vector<glm::vec2>& t_coord,
		std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>>& is_vertex,
		uint32_t f,
		uint32_t v_i
	);
};

