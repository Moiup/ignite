#include "Pixels.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

Pixels::Pixels()
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

	data = stbi_load(file_path.c_str(), &width, &height, &n, _nb_channels);

	if (!data) {
		std::cerr << "Error: failed opening the texture '" << file_path << "'" << std::endl;
		return false;
	}

	_pixels = new uint8_t[width * height * _nb_channels];
	_width = width;
	_height = height;

	//for (uint32_t i = 0; i < width * height; i++) {
	//	_pixels[i].r = data[i * n];
	//	_pixels[i].g = data[i * n + 1];
	//	_pixels[i].b = data[i * n + 2];
	//	_pixels[i].a = data[i * n + 3];
	//}

	std::memcpy(_pixels.data(), data, width * height * _nb_channels * sizeof(uint8_t));

	stbi_image_free(data);

	return true;
}

void Pixels::setPixels(uint32_t width, uint32_t height) {
	_width = width;
	_height = height;
	_pixels = new uint8_t[_width * _height * _nb_channels];
	memset(_pixels.data(), 0, _width * _height * _nb_channels * sizeof(uint8_t));
}

void Pixels::setPixels(Pointer<uint8_t>& pixels, uint32_t width, uint32_t height) {
	_width = width;
	_height = height;
	_pixels = pixels;
}

uint32_t Pixels::getWidth() {
	return _width;
}

uint32_t Pixels::getHeight() {
	return _height;
}

glm::vec4 Pixels::getPixel(uint32_t l, uint32_t c) {
	return glm::vec4(
		_pixels[pixR(l, c)],
		_pixels[pixG(l, c)],
		_pixels[pixB(l, c)],
		_pixels[pixA(l, c)]
	);
}

Pointer<uint8_t>& Pixels::getPixels() {
	return _pixels;
}

uint32_t Pixels::pixIndex(uint32_t l, uint32_t c) {
	return (l * _width + c) * _nb_channels;
}

uint32_t Pixels::pixR(uint32_t l, uint32_t c) {
	return pixIndex(l, c) + 1;
}

uint32_t Pixels::pixG(uint32_t l, uint32_t c) {
	return pixIndex(l, c) + 2;
}

uint32_t Pixels::pixB(uint32_t l, uint32_t c) {
	return pixIndex(l, c) + 3;
}
