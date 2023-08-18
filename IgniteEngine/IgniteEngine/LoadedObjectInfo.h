#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "fast_obj/fast_obj.h"

struct LoadedObjectInfo
{
	Mesh _mesh;
	std::vector<Material> _materials;

	std::vector<LoadedObjectInfo> _children;

	LoadedObjectInfo();

	void loadObj(const std::string& file_name);
};

