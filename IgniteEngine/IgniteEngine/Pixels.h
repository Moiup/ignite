#pragma once

#include "Pointer.h"

#include <cstdint>
#include <iostream>
#include <string>

#include <vector>
#include <glm/glm.hpp>

class Pixels {
private:
	uint32_t _width{};
	uint32_t _height{};
	int32_t _nb_channels{ 4 };
	uint8_t _pix_size{};

	Pointer<uint8_t> _pixels;

public:
	Pixels();
	Pixels(std::string file_path);
	//Pixels(Pointer<glm::vec4>& pixels, uint32_t width, uint32_t height);
	~Pixels();

	bool readFile(std::string file_path);
	void setPixels(uint32_t width, uint32_t height);
	void setPixels(Pointer<uint8_t>& pixels, uint32_t width, uint32_t height);

	uint32_t getWidth();
	uint32_t getHeight();
	glm::vec4 getPixel(uint32_t l, uint32_t c);
	void setPixel(uint32_t l, uint32_t c, glm::vec4 pix);
	void setPixel(uint32_t l, uint32_t c, float r, float g, float b, float a);
	void setPixel(uint32_t l, uint32_t c, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void setPixel(uint32_t l, uint32_t c, float col);
	void setPixel(uint32_t l, uint32_t c, uint8_t col);


	Pointer<uint8_t>& getPixels();

private:
	uint32_t pixIndex(uint32_t l, uint32_t c);
	uint32_t pixR(uint32_t l, uint32_t c);
	uint32_t pixG(uint32_t l, uint32_t c);
	uint32_t pixB(uint32_t l, uint32_t c);
	uint32_t pixA(uint32_t l, uint32_t c);
};