#include "Texture.h"

Texture::Texture() : 
	_pixels{},
	_width{ 0 },
	_height{ 0 }
{
	;
}

Texture::Texture(std::string file_name) {
	readFile(file_name);
}

Texture::Texture(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height) {
	setPixels(pixels, width, height);
}

glm::vec4& Texture::pixel(uint64_t row, uint64_t col) {
	return _pixels[row * _width + col];
}

const std::vector<glm::vec4>& Texture::pixels() const {
	return _pixels;
}

const glm::vec4& Texture::getPixel(uint64_t row, uint64_t col) {
	glm::vec4& p = pixel(row, col);
	return p;
}

/**
* Bilinear interpolation to get the corresponding color.
* 
*/
const glm::vec4& Texture::getPixel(float u, float v) const {
	assert(u > 1.0 && v > 1.0, "Error: u and v must be inferior or equal to 1\n");
	assert(u < 0 && v < 0, "Error: u and v must be superior or equal to 0.\n");
	
	float p_u = _height - u * _height;
	float p_v = _width - v * _width;
	
	uint64_t p00u = static_cast<uint64_t>(p_u);
	uint64_t p00v = static_cast<uint64_t>(p_v);

	uint64_t p10u = static_cast<uint64_t>(p_u) + 1;
	uint64_t p10v = static_cast<uint64_t>(p_v);

	uint64_t p01u = static_cast<uint64_t>(p_u);
	uint64_t p01v = static_cast<uint64_t>(p_v) + 1;

	uint64_t p11u = static_cast<uint64_t>(p_u) + 1;
	uint64_t p11v = static_cast<uint64_t>(p_v) + 1;
	
	float diff{};

	glm::vec4 t_pix{};
	glm::vec4 b_pix{};
	glm::vec4 pix{};

	if (p00u == _height) {
		// we do not have the top neighbor
		p10u = p00u;
		p11u = p00u;
	}
	if(p00v == _width){
		// we do not have the right neighbor
		p11v = p00v;
		p01v = p00v;
	}

	diff = p_v - p00v;
	t_pix = diff * getPixel(p11u, p11v) + (1 - diff) * getPixel(p10u, p10v);
	b_pix = diff * getPixel(p01u, p01v) + (1 - diff) * getPixel(p00u, p00v);

	diff = p_u - p00u;
	pix = diff * t_pix + (1 - diff) * b_pix;

	return pix;
}

const glm::vec4& Texture::getPixel(glm::vec2 uv) const {
	return getPixel(uv.r, uv.g);
}

void Texture::setPixel(glm::vec4& pix, uint64_t row, uint64_t col){
	pixel(row, col) = pix;
}

void Texture::setPixels(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height) {
	_pixels = pixels;
	_width = width;
	_height = height;
	_width_inv = 1.0f / width;
	_height_inv = 1.0f / height;
}

bool Texture::readFile(std::string file_name) {
	unsigned char* data;
	std::vector<glm::vec4> pixels;
	int width, height, n;

	data = stbi_load(file_name.c_str(), &width, &height, &n, 4);
	if (!data) {
		std::cerr << "Error: failed opening the texture '" << file_name << "'" << std::endl;
		return false;
	}

	std::memcpy(pixels.data(), data, width * height * 4 * sizeof(*data));
	stbi_image_free(data);

	setPixels(pixels, width, height);
	
	return true;
}

const uint64_t Texture::getWidth() const {
	return _width;
}

const uint64_t Texture::getHeight() const {
	return _height;
}