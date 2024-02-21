#include "Pixels.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

Pixels::Pixels() :
	_width{0},
	_height{0},
	_pixels{}
{
	;
}

Pixels::Pixels(std::string file_path) {
	readFile(file_path);
}

//Pixels::Pixels(Pointer<glm::vec4>& pixels, uint32_t width, uint32_t height) {
//	setPixels(pixels, width, height);
//}

Pixels::~Pixels(){
	;
}

bool Pixels::readFile(std::string file_path) {
	uint8_t* data;
	int width, height, n;

	data = stbi_load(file_path.c_str(), &width, &height, &n, _n);

	if (!data) {
		std::cerr << "Error: failed opening the texture '" << file_path << "'" << std::endl;
		return false;
	}

	_pixels = new uint8_t[width * height * _n];
	_width = width;
	_height = height;

	//for (uint32_t i = 0; i < width * height; i++) {
	//	_pixels[i].r = data[i * n];
	//	_pixels[i].g = data[i * n + 1];
	//	_pixels[i].b = data[i * n + 2];
	//	_pixels[i].a = data[i * n + 3];
	//}

	std::memcpy(_pixels.data(), data, width * height * _n * sizeof(uint8_t));

	stbi_image_free(data);

	return true;
}

//void Pixels::setPixels(uint32_t width, uint32_t height) {
//	_width = width;
//	_height = _height;
//	//delete[] _pixels;
//	_pixels = new glm::vec4[width * height];
//}

//void Pixels::setPixels(Pointer<glm::vec4>& pixels, uint32_t width, uint32_t height) {
//	_width = width;
//	_height = height;
//	//delete[] _pixels;
//	_pixels = new glm::uint8_t[width * height * _n];
//	std::memcpy(&_pixels[0], &pixels[0], width * height * sizeof(glm::vec4));
//}

uint32_t Pixels::getWidth() {
	return _width;
}

uint32_t Pixels::getHeight() {
	return _height;
}

glm::vec4 Pixels::getPixel(uint32_t r, uint32_t c) {
	return glm::vec4(
		_pixels[(r * _width + c) * _n],
		_pixels[(r * _width + c) * _n + 1],
		_pixels[(r * _width + c) * _n + 2],
		_pixels[(r * _width + c) * _n + 3]
	);
}

Pointer<uint8_t>& Pixels::getPixels() {
	return _pixels;
}