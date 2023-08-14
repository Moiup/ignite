#include "Mesh.h"

std::vector<Mesh*> Mesh::meshes;

Mesh::Mesh() :
	_coords{},
	_indices{},
	_uv{},
	_normals{}
{ 
	//Mesh::meshes.push_back(this);
}

Mesh::Mesh(
	const std::vector<glm::vec3>& coords,
	const std::vector<uint32_t>& indices
) :
	_uv{},
	_normals{}
{
	setVertex(coords, indices);
}

void Mesh::setVertex(
	const std::vector<glm::vec3>& coords,
	const std::vector<uint32_t>& indices
) {
	_coords = coords;
	_indices = indices;
}

void Mesh::setVertex(
	const std::vector<glm::vec3>& coords,
	const std::vector<uint32_t>& indices,
	const std::vector<glm::vec2>& uv
) {
	_coords = coords;
	_indices = indices;
	_uv = uv;
}

void Mesh::setVertex(
	const std::vector<glm::vec3>& coords,
	const std::vector<uint32_t>& indices,
	const std::vector<glm::vec2>& uv,
	const std::vector<glm::vec3>& normals
) {
	_coords = coords;
	_indices = indices;
	_uv = uv;
	_normals = normals;
}

const void Mesh::setCoords(float* coords, const uint32_t len) {
	glm::vec3* c = (glm::vec3*)coords;
	_coords.assign(c, c + len);
}

const void Mesh::setIndices(uint32_t* indices, const uint32_t len) {
	_indices.assign(indices, indices + len);
}

const void Mesh::setUV(float* uv, const uint32_t len) {
	glm::vec2* t = (glm::vec2*)uv;
	_uv.assign(t, t + len/2);
}

const void Mesh::setNormals(float* normals, const uint32_t len) {
	glm::vec3* n = (glm::vec3*)normals;
	_normals.assign(n, n + len/3);
}

const void Mesh::setIndicesToMaterial(uint32_t* indices_mat, const uint32_t len) {
	_indices_to_mat.assign(indices_mat, indices_mat + len);
}

const void Mesh::setMaterials(fastObjMaterial* mat, const uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		_materials.push_back(Material(mat[i]));
	}
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

const std::vector<glm::vec3>& Mesh::getNormals() const {
	return _normals;
}

const std::vector<uint32_t>& Mesh::getIndicesToMaterial() const {
	return _indices_to_mat;
}

const std::vector<Material>& Mesh::getMaterials() const {
	return _materials;
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

uint32_t Mesh::getNormalsStride() {
	return sizeof(*_normals.data());
}

uint32_t Mesh::getIndicesToMaterialStride() {
	return sizeof(*_indices_to_mat.data());
}

uint32_t Mesh::getMaterialsStride() {
	return sizeof(*_materials.data());
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

uint32_t Mesh::getNormalsNbElem() {
	return _normals.size();
}

uint32_t Mesh::getIndicesToMaterialNbElem() {
	return _indices_to_mat.size();
}

uint32_t Mesh::getMaterialsNbElem() {
	return _materials.size();
}