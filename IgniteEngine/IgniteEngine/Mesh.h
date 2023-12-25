#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Material.h"

class Mesh
{
private:
	static std::vector<Mesh*> meshes;

	std::vector<glm::vec3> _coords;
	std::vector<uint32_t> _indices;
	std::vector<glm::vec2> _uv;
	std::vector<glm::vec3> _normals;

public:
	Mesh();
	Mesh(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices
	);

	void setVertex(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices
	);

	void setVertex(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices,
		const std::vector<glm::vec2>& uv
	);

	void setVertex(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices,
		const std::vector<glm::vec2>& uv,
		const std::vector<glm::vec3>& normals
	);

	const void setCoords(float* coords, const uint32_t len);
	const void setIndices(uint32_t* indices, const uint32_t len);
	const void setUV(float* uv, const uint32_t len);
	const void setNormals(float* normals, const uint32_t len);

	const std::vector<glm::vec3>& getCoords() const;
	const std::vector<uint32_t>& getIndices() const;
	const std::vector<glm::vec2>& getUV() const;
	const std::vector<glm::vec3>& getNormals() const;

	uint32_t getCoordsSize();
	uint32_t getCoordsStride();
	uint32_t getUVStride();
	uint32_t getNormalsStride();

	uint32_t getIndicesSize();
	uint32_t getIndicesNbElem();
	uint32_t getUVNbElem();
	uint32_t getNormalsNbElem();
};

