#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "fast_obj/fast_obj.h"

struct LoadedObjectInfo
{
	std::vector<Mesh> _meshes;
	std::vector<std::vector<uint32_t>> _material_indices; // array of indices for each mesh
	std::vector<std::vector<Material>> _materials;
	std::vector<LoadedObjectInfo> _children;

	LoadedObjectInfo();

	void loadWavefont(const std::string& file_name);
};

