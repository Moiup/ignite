#include "Pixels.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

Pixels::Pixels() :
	_width{0},
	_height{0},
	_pixels{nullptr}
{
	;
}

Pixels::~Pixels(){
	delete[] _pixels;
}

bool Pixels::readFile(std::string file_path) {
	uint8_t* data;
	int width, height, n;

	data = stbi_load(file_path.c_str(), &width, &height, &n, _n);
	if (!data) {
		std::cerr << "Error: failed opening the texture '" << file_path << "'" << std::endl;
		return false;
	}

	delete[] _pixels;

	_pixels = new glm::vec4[width * height];
	_width = width;
	_height = height;

	std::memcpy(_pixels, data, width * height * _n * sizeof(uint8_t));

	stbi_image_free(data);

	return true;
}

void Pixels::setPixels(uint32_t width, uint32_t height) {
	_width = width;
	_height = _height;
	delete[] _pixels;
	_pixels = new glm::vec4[width * height];
}

void Pixels::setPixels(glm::vec4* pixels, uint32_t width, uint32_t height) {
	_width = width;
	_height = height;
	delete[] _pixels;
	_pixels = static_cast<glm::vec4*>(std::memcpy(_pixels, pixels, width * height * sizeof(glm::vec4)));
}

uint32_t Pixels::getWidth() {
	return _width;
}

uint32_t Pixels::getHeight() {
	return _height;
}

glm::vec4& Pixels::getPixel(uint32_t r, uint32_t c) {
	return _pixels[r * _width + c];
}

glm::vec4* Pixels::getPixels() {
	return _pixels;
}