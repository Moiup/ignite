#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

#include <cstdint>
#include <iostream>
#include <string>

#include <vector>
#include <glm/glm.hpp>

class Texture
{
private:
	std::vector<glm::vec4> _pixels;
	uint64_t _width;
	uint64_t _height;
	float _width_inv;
	float _height_inv;

public:
	Texture();
	Texture(std::string file_name);
	Texture(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height);

	glm::vec4& pixel(uint64_t row, uint64_t col);
	const std::vector<glm::vec4>& pixels() const;

	const glm::vec4& getPixel(uint64_t row, uint64_t col);
	const glm::vec4& getPixel(float u, float v) const;
	const glm::vec4& getPixel(glm::vec2 uv) const;

	void setPixel(glm::vec4& pix, uint64_t row, uint64_t col);
	void setPixels(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height);

	bool readFile(std::string file_name);

	const uint64_t getWidth() const;
	const uint64_t getHeight() const;
};

