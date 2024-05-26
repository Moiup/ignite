#include "Texture.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//
//#include "stb_image/stb_image.h"
//#include "stb_image/stb_image_write.h"

Texture::Texture() : 
	_width{ 0 },
	_height{ 0 },
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

void Texture::setSampler(Sampler& sampler) {
	_sampler = sampler.getSampler();
}

VkSampler Texture::getSampler() {
	return _sampler;
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
	Image::setQueue(_queue);
	Image::setImageImageType(VK_IMAGE_TYPE_2D);
	Image::setImageFormat(_format);
	Image::setImageMipLevels(1);
	Image::setImageArrayLayers(1);
	Image::setImageSamples(VK_SAMPLE_COUNT_1_BIT);
	Image::setImageTiling(VK_IMAGE_TILING_OPTIMAL);
	Image::setImageSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	Image::setImageInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
	Image::setImageExtent(_width, _height, 1);
	Image::setImageUsage(
		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT |
		VK_IMAGE_USAGE_STORAGE_BIT
	);
	Image::setMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Image::createImage();
	Image::allocateMemory();
	Image::bind();

	Image::setImageViewViewType(VK_IMAGE_VIEW_TYPE_2D);
	Image::setImageViewFormat(_format);
	Image::setImageViewSurbresourceRange(
		VK_IMAGE_ASPECT_COLOR_BIT,
		0, // base mip level
		1, // level count
		0, // base array layer
		1  // layer count
	);
	Image::createImageView();

	// Creating the staging buffer
	_staging_buffer.setQueue(_queue);
	_staging_buffer.setSize(_width * _height * _n * sizeof(uint8_t));
	_staging_buffer.create();
}

void Texture::update(Pixels& pixels) {
	// Copying the actual texture data into the staging buffer
	_staging_buffer.setValues(pixels.getPixels().data());

	changeLayout(
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_HOST_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	// Copying the data from the buffer to the image
	copy(_staging_buffer);
}

void Texture::copy(Buffer& buffer) {
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

	CommandBuffer cmd_buff = _queue->allocateCommandBuffer();
	cmd_buff.begin();

	cmd_buff.copyBufferToImage(
		_staging_buffer.getBuffer(),
		Image::getImage(),
		Image::getImageLayout(),
		1,
		&image_copy
	);

	cmd_buff.end();

	changeLayout(
		_image_info.initialLayout,
		_stage_access_info.access_mask,
		VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
		_stage_access_info.stage_mask,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
	);

	//VkImageSubresourceRange image_barrier_sub_range{};
	//image_barrier_sub_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//image_barrier_sub_range.baseMipLevel = 0;
	//image_barrier_sub_range.levelCount = 1;
	//image_barrier_sub_range.layerCount = 0;
	//image_barrier_sub_range.layerCount = 1;

	//VkImageMemoryBarrier image_barrier{};
	//image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//image_barrier.pNext = nullptr;
	//image_barrier.srcAccessMask = _stage_access_info.access_mask;
	//image_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
	//image_barrier.oldLayout = _image_info.initialLayout;
	//image_barrier.newLayout = _image_info.initialLayout;
	//image_barrier.image = Image::getImage();
	//image_barrier.subresourceRange = image_barrier_sub_range;

	//cmd_buff.pipelineBarrier(
	//	_stage_access_info.stage_mask,
	//	VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
	//	0,
	//	0, nullptr,
	//	0, nullptr,
	//	1, &image_barrier
	//);

	//cmd_buff.end();

	//_stage_access_info.access_mask = image_barrier.dstAccessMask;
	//_stage_access_info.stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
}

void Texture::flushToStaging() {
	_staging_buffer.copy(*this);
}

void Texture::flushPixels(Pixels& pixels) {
	flushToStaging();
	_queue->submit();
	_queue->wait();
	_staging_buffer.getValues(pixels.getPixels().data());
}

void Texture::destroy() {
	Image::freeMemory();
	Image::destroyImage();
	Image::destroyImageView();
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
	return Image::getImageExtentWidth();
}

const uint64_t Texture::getHeight() const {
	return Image::getImageExtentHeight();
}

const uint8_t Texture::getNbComponents() const {
	return _n;
}

//const Image& Texture::getImage() const {
//	return _image;
//}