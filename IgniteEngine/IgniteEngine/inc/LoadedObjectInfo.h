#pragma once

#include "Mesh.h"
#include "Texture2D.h"
#include "Material.h"
#include "Skeleton.h"
#include "DefaultConf.h"
#include "StagingBuffer.h"
#include "fast_obj/fast_obj.h"
#include <iostream>
#include <fstream>

#include "tiny_gltf.h"

struct GLTFBuffInfo {
	unsigned char* _data;
	uint64_t _byte_length;
	uint64_t _count;
	int _component_type;
};

struct LoadedObjectInfo
{
	std::vector<Mesh> _meshes;
	std::vector<Skeleton> _skeletons;
	std::vector<std::vector<Joint>> _joints;
	std::vector<std::vector<uint32_t>> _material_indices; // array of indices for each mesh
	std::vector<std::vector<Material>> _materials;
	std::vector<std::vector<Texture2D>> _textures;
	std::vector<LoadedObjectInfo> _children;

	LoadedObjectInfo();

	void loadWavefont(const std::string& file_name);
	void loadGLTF(const std::string& file_name);

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

	GLTFBuffInfo retreiveBufferDataGltf(const tinygltf::Model& model, int accessor_index);
};

