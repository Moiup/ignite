#pragma once

//#include <iostream>
#include <cstdint>
#include <iostream>
#include <string>

#include <vector>
#include <glm/glm.hpp>

class Pixels {
private:
	uint32_t _width;
	uint32_t _height;
	uint8_t _n{ 4 };

	glm::vec4* _pixels;

public:
	Pixels();
	Pixels(std::string file_path);
	Pixels(glm::vec4* pixels, uint32_t width, uint32_t height);
	~Pixels();

	bool readFile(std::string file_path);
	void setPixels(uint32_t width, uint32_t height);
	void setPixels(glm::vec4* pixels, uint32_t width, uint32_t height);

	uint32_t getWidth();
	uint32_t getHeight();
	glm::vec4& getPixel(uint32_t r, uint32_t c);


	glm::vec4* getPixels();
};