#include "Mesh.h"

Mesh::Mesh() :
	_coords{},
	_indices{},
	_uv{}
{ ; }

Mesh::Mesh(
	const std::vector<glm::vec3>& coords,
	const std::vector<uint32_t>& indices
) {
	setCoords(coords, indices);
}

void Mesh::setCoords(
	const std::vector<glm::vec3>& coords,
	const std::vector<uint32_t>& indices
) {
	_coords = coords;
	_indices = indices;
}

const std::vector<glm::vec3>& Mesh::getCoords() const {
	return _coords;
}

const std::vector<uint32_t>& Mesh::getIndices() const {
	return _indices;
}

const std::vector<glm::vec2>& Mesh::getUV() const {
	return _uv;
}

uint32_t Mesh::getCoordsSize() {
	return _coords.size() * sizeof(*_coords.data());
}

uint32_t Mesh::getCoordsStride() {
	return sizeof(*_coords.data());
}

uint32_t Mesh::getUVStride() {
	return sizeof(*_uv.data());
}

uint32_t Mesh::getIndicesSize() {
	return _indices.size() * sizeof(*_indices.data());
}

uint32_t Mesh::getIndicesNbElem() {
	return _indices.size();
}

uint32_t Mesh::getUVNbElem() {
	return _uv.size();
}