#include "Texture.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

Texture::Texture() : 
	_pixels{},
	_width{ 0 },
	_height{ 0 },
	_n{4},
	_width_inv{ 0 },
	_height_inv{ 0 }
{
	;
}

Texture::Texture(std::string file_name) :
	Texture::Texture()
{
	readFile(file_name);
}

Texture::Texture(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height) :
	Texture::Texture()
{
	setPixels(pixels, width, height);
}

void Texture::setLogicalDevice(LogicalDevice* logical_device) {
	_logical_device = logical_device;
}

void Texture::setGPU(PhysicalDevice* gpu) {
	_gpu = gpu;
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

void Texture::create() {
	// Creating the staging buffer
	Buffer staging_buffer{};
	staging_buffer.setLogicalDevice((VkDevice*)_logical_device->getDevice());
	staging_buffer.setMemoryProperties(_gpu->getMemoryProperties());
	staging_buffer.setFlags(0);
	staging_buffer.setPNext(nullptr);
	staging_buffer.setQueueFamilyIndexCount(0);
	staging_buffer.setPQueueFamilyIndices(nullptr);
	staging_buffer.setSize(_width * _height * _n * sizeof(_pixels.data()));
	staging_buffer.setUsage(VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	staging_buffer.setSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	staging_buffer.create();

	staging_buffer.allocateMemory(
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
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

void Texture::setPixels(std::vector<glm::vec4> pixels, uint64_t width, uint64_t height) {
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

	data = stbi_load(file_name.c_str(), &width, &height, &n, _n);
	if (!data) {
		std::cerr << "Error: failed opening the texture '" << file_name << "'" << std::endl;
		return false;
	}
	size_t nb_elem = width * height * _n;
	size_t data_size = nb_elem * sizeof(*data);
	pixels.resize(data_size);
	std::memcpy(pixels.data(), (float *)data, data_size);

	stbi_image_free(data);

	setPixels(pixels, width, height);
	
	return true;
}

bool Texture::writeFile(std::string file_name) {
	return stbi_write_png(
		file_name.c_str(),
		_width,
		_height,
		4,
		(unsigned char*)_pixels.data(),
		4 * _width
	);
}

const uint64_t Texture::getWidth() const {
	return _width;
}

const uint64_t Texture::getHeight() const {
	return _height;
}