#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class Mesh
{
private:
	std::vector<glm::vec3> _coords;
	std::vector<uint32_t> _indices;
	std::vector<glm::vec2> _uv;

public:
	Mesh();
	Mesh(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices
	);

	void setCoords(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices
	);

	void setCoords(
		const std::vector<glm::vec3>& coords,
		const std::vector<uint32_t>& indices,
		const std::vector<glm::vec2>& uv
	);
	
	const std::vector<glm::vec3>& getCoords() const;
	const std::vector<uint32_t>& getIndices() const;
	const std::vector<glm::vec2>& getUV() const;

	uint32_t getCoordsSize();
	uint32_t getCoordsStride();
	uint32_t getUVStride();

	uint32_t getIndicesSize();
	uint32_t getIndicesNbElem();
	uint32_t getUVNbElem();
};

