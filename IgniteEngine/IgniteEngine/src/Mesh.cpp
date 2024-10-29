#include "Mesh.h"

Mesh::Mesh() :
	_coords{},
	_indices{},
	_uv{},
	_normals{}
{

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
	glm::vec3* c = reinterpret_cast<glm::vec3*>(coords);
	_coords.assign(c, c + len);
}

const void Mesh::setIndices(uint32_t* indices, const uint32_t len) {
	_indices.assign(indices, indices + len);
}

const void Mesh::setUV(float* uv, const uint32_t len) {
	glm::vec2* t = reinterpret_cast<glm::vec2*>(uv);
	_uv.assign(t, t + len);
}

const void Mesh::setNormals(float* normals, const uint32_t len) {
	glm::vec3* n = reinterpret_cast<glm::vec3*>(normals);
	_normals.assign(n, n + len);
}

const void Mesh::setJoints(uint32_t* joints, const uint32_t len) {
	glm::uvec4* j = reinterpret_cast<glm::uvec4*>(joints);
	_joints.assign(j, j + len);
}

const void Mesh::setWeights(float* weights, const uint32_t len) {
	glm::vec4* w = reinterpret_cast<glm::vec4*>(weights);
	_weights.assign(w, w + len);
}

const std::vector<glm::vec3>& Mesh::getCoords() const {
	return _coords;
}

std::vector<glm::vec3>& Mesh::getCoords() {
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

const std::vector<glm::uvec4>& Mesh::getJoints() const {
	return _joints;
}

const std::vector<glm::vec4>& Mesh::getWeights() const {
	return _weights;
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

uint32_t Mesh::getJointsStride() {
	return sizeof(*_joints.data());
}

uint32_t Mesh::getWeightsStride() {
	return sizeof(*_weights.data());
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

uint32_t Mesh::getJointsNbElem() {
	return _joints.size();
}

uint32_t Mesh::getWeightsNbElem() {
	return _weights.size();
}