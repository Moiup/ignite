#include "LoadedObjectInfo.h"

LoadedObjectInfo::LoadedObjectInfo() { ; }

void LoadedObjectInfo::loadObj(const std::string& file_name) {
	fastObjMesh* fom = fast_obj_read(file_name.c_str());

	if (!fom) {
		std::cout << "Error opening the file: " << file_name << std::endl;
		throw std::runtime_error("Error opening file.");
	}

	// Retreiving indices value
	std::vector<uint32_t> indices(fom->index_count);
	for (uint32_t i = 0; i < fom->index_count; i++) {
		indices[i] = fom->indices[i].p;
	}

	_mesh.setCoords(fom->positions, fom->position_count);
	_mesh.setNormals(fom->normals, fom->normal_count);
	_mesh.setUV(fom->texcoords, fom->texcoord_count);
	_mesh.setIndices(indices.data(), fom->face_count);

	fast_obj_destroy(fom);
}