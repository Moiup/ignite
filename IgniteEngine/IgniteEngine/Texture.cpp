#include "Texture.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//
//#include "stb_image/stb_image.h"
//#include "stb_image/stb_image_write.h"

Texture::Texture() : 
	_width{ 0 },
	_height{ 0 },
	_image{},
	_image_layout{},
	_format{ VK_FORMAT_R8G8B8A8_UNORM }
{
	;
}

//Texture::Texture(std::string file_name) :
//	Texture::Texture()
//{
//	readFile(file_name);
//}

//Texture::Texture(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height) :
//	Texture::Texture()
//{
//	setPixels(pixels, width, height);
//}
//
//Texture::Texture(uint64_t width, uint64_t height) :
//	Texture::Texture()
//{
//	std::vector<glm::vec4> pixels(width * height * _n);
//	setPixels(pixels, width, height);
//}

//void Texture::setDevice(Device* _device) {
//	_device = _device;
//}
//
//void Texture::setGPU(PhysicalDevice* gpu) {
//	_gpu = gpu;
//}
//
//void Texture::setCommandPool(CommandPool* command_pool) {
//	_command_pool = command_pool;
//}

void Texture::setQueue(Queue* queue) {
	_queue = queue;
}

void Texture::setFormat(VkFormat format) {
	_format = format;
}

void Texture::setDimensions(uint32_t width, uint32_t height) {
	_width = width;
	_height = height;
}

//glm::vec4& Texture::pixel(uint64_t row, uint64_t col) {
//	return _pixels[row * _width + col];
//}
//
//const std::vector<glm::vec4>& Texture::pixels() const {
//	return _pixels;
//}

//const glm::vec4& Texture::getPixel(uint64_t row, uint64_t col) {
//	glm::vec4& p = pixel(row, col);
//	return p;
//}

void Texture::create() {
	// Create the image to copy the buffer to
	_image.setQueue(_queue);
	_image.setImageImageType(VK_IMAGE_TYPE_2D);
	_image.setImageFormat(_format);
	_image.setImageMipLevels(1);
	_image.setImageArrayLayers(1);
	_image.setImageSamples(VK_SAMPLE_COUNT_1_BIT);
	_image.setImageTiling(VK_IMAGE_TILING_OPTIMAL);
	_image.setImageSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	_image.setImageInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
	_image.setImageExtent(_width, _height, 1);
	_image.setImageUsage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	_image.setMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	_image.createImage();
	_image.allocateMemory();
	_image.bind();

	_image.setImageViewViewType(VK_IMAGE_VIEW_TYPE_2D);
	_image.setImageViewFormat(_format);
	_image.setImageViewSurbresourceRange(
		VK_IMAGE_ASPECT_COLOR_BIT,
		0, // base mip level
		1, // level count
		0, // base array layer
		1  // layer count
	);
	_image.createImageView();

	// Creating the staging buffer
	_staging_buffer.setQueue(_queue);
	_staging_buffer.setSize(_width * _height * _n * sizeof(uint8_t));
	_staging_buffer.create();
}

void Texture::update(Pixels& pixels) {
	// Copying the actual texture data into the staging buffer
	_staging_buffer.setValues(pixels.getPixels().data());

	// To do for each mip level
	// (To start, we consider only the original level -> 0)
	VkBufferImageCopy image_copy{};
	image_copy.bufferOffset = 0;
	image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_copy.imageSubresource.mipLevel = 0;
	image_copy.imageSubresource.baseArrayLayer = 0;
	image_copy.imageSubresource.layerCount = 1;
	image_copy.imageExtent.width = _width;
	image_copy.imageExtent.height = _height;
	image_copy.imageExtent.depth = 1;

	// Preparing the transfer with the image memory barrier
	VkImageSubresourceRange subresource_range{};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.layerCount = 1;

	VkImageMemoryBarrier image_memory_barrier{};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.pNext = nullptr;
	image_memory_barrier.image = _image.getImage();
	image_memory_barrier.subresourceRange = subresource_range;
	image_memory_barrier.srcAccessMask = 0;
	image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	image_memory_barrier.oldLayout = _image_layout;
	image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	// Creating the command buffer
	CommandBuffer copy_cmd = _queue->allocateCommandBuffer();
	copy_cmd.begin();

	copy_cmd.pipelineBarrier(
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier
	);

	// Copying the data from the buffer to the image
	copy_cmd.copyBufferToImage(
		_staging_buffer.getBuffer(),
		_image.getImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&image_copy
	);

	// transfering the image to the shader read layout so it can be sampled from
	image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	image_memory_barrier.newLayout = _image_layout;

	copy_cmd.pipelineBarrier(
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &image_memory_barrier
	);

	copy_cmd.end();

	//_queue->submit();
	//_queue->wait();
}

void Texture::destroy() {
	_image.freeMemory();
	_image.destroyImage();
	_image.destroyImageView();
}

/**
* Bilinear interpolation to get the corresponding color.
* 
*/
//const glm::vec4& Texture::getPixel(float u, float v) const {
//	assert(u > 1.0 && v > 1.0, "Error: u and v must be inferior or equal to 1\n");
//	assert(u < 0 && v < 0, "Error: u and v must be superior or equal to 0.\n");
//	
//	float p_u = _height - u * _height;
//	float p_v = _width - v * _width;
//	
//	uint64_t p00u = static_cast<uint64_t>(p_u);
//	uint64_t p00v = static_cast<uint64_t>(p_v);
//
//	uint64_t p10u = static_cast<uint64_t>(p_u) + 1;
//	uint64_t p10v = static_cast<uint64_t>(p_v);
//
//	uint64_t p01u = static_cast<uint64_t>(p_u);
//	uint64_t p01v = static_cast<uint64_t>(p_v) + 1;
//
//	uint64_t p11u = static_cast<uint64_t>(p_u) + 1;
//	uint64_t p11v = static_cast<uint64_t>(p_v) + 1;
//	
//	float diff{};
//
//	glm::vec4 t_pix{};
//	glm::vec4 b_pix{};
//	glm::vec4 pix{};
//
//	if (p00u == _height) {
//		// we do not have the top neighbor
//		p10u = p00u;
//		p11u = p00u;
//	}
//	if(p00v == _width){
//		// we do not have the right neighbor
//		p11v = p00v;
//		p01v = p00v;
//	}
//
//	diff = p_v - p00v;
//	t_pix = diff * getPixel(p11u, p11v) + (1 - diff) * getPixel(p10u, p10v);
//	b_pix = diff * getPixel(p01u, p01v) + (1 - diff) * getPixel(p00u, p00v);
//
//	diff = p_u - p00u;
//	pix = diff * t_pix + (1 - diff) * b_pix;
//
//	return pix;
//}
//
//const glm::vec4& Texture::getPixel(glm::vec2 uv) const {
//	return getPixel(uv.r, uv.g);
//}
//
//void Texture::setPixel(glm::vec4& pix, uint64_t row, uint64_t col){
//	pixel(row, col) = pix;
//}

//void Texture::setPixels(std::vector<glm::vec4> pixels, uint64_t width, uint64_t height) {
//	_pixels = pixels;
//	_width = width;
//	_height = height;
//	_width_inv = 1.0f / width;
//	_height_inv = 1.0f / height;
//}

//void Texture::setPixels(void* pixels, uint64_t width, uint64_t height, uint8_t n) {
//	size_t nb_elem = width * height * n;
//	size_t data_size = nb_elem;
//	_pixels.resize(data_size);
//	std::memcpy(_pixels.data(), pixels, data_size);
//	_width = width;
//	_height = height;
//	_width_inv = 1.0f / width;
//	_height_inv = 1.0f / height;
//}

//bool Texture::readFile(std::string file_name) {
//	uint8_t* data;
//	int width, height, n;
//
//	data = stbi_load(file_name.c_str(), &width, &height, &n, _n);
//	if (!data) {
//		std::cerr << "Error: failed opening the texture '" << file_name << "'" << std::endl;
//		return false;
//	}
//
//	_pixels.resize(width * height);
//	_width = width;
//	_height = height;
//
//	std::memcpy(_pixels.data(), data, width * height * _n) ;
//
//	stbi_image_free(data);
//
//	_format = VK_FORMAT_R8G8B8A8_UNORM;
//
//	return true;
//}

//bool Texture::writeFile(std::string file_name) {
//	std::vector<uint8_t> data(_width * _height * _n);
//	uint32_t len = _width * _height;
//
//	for (uint32_t i = 0, j = 0; i < len; i++, j+=4) {
//		data[j] = _pixels[i].r;
//		data[j + 1] = _pixels[i].g;
//		data[j + 2] = _pixels[i].b;
//		data[j + 3] = _pixels[i].a;
//	}
//
//	return stbi_write_bmp(
//		file_name.c_str(),
//		_width,
//		_height,
//		_n,
//		data.data()
//	);
//}
//
//bool Texture::writeFileHDR(std::string file_name) {
//	return stbi_write_hdr(
//		file_name.c_str(),
//		_width,
//		_height,
//		_n,
//		&_pixels[0].r
//	);
//}

const uint32_t Texture::getWidth() const {
	return _image.getImageExtentWidth();
}

const uint64_t Texture::getHeight() const {
	return _image.getImageExtentHeight();
}

const uint8_t Texture::getNbComponents() const {
	return _n;
}

const Image& Texture::getImage() const {
	return _image;
}