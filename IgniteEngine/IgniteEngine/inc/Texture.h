#pragma once

#include "Device.h"
#include "Queue.h"
#include "Image.h"
#include "PhysicalDevice.h"
#include "Pixels.h"
#include "Sampler.h"

#include <cstdint>
#include <iostream>
#include <string>

#include <vector>
#include <glm/glm.hpp>


class Texture : public Image
{
private:

	Queue* _queue;

	//Image _image;
	//VkImageLayout _image_layout{ VK_IMAGE_LAYOUT_UNDEFINED };

	VkFormat _format;

	VkSampler _sampler;


public:
	Texture();

	void setQueue(Queue* queue);
	void setFormat(VkFormat format);
	void setDimensions(uint32_t width, uint32_t height);
	void setSampler(Sampler& sampler);

	VkSampler getSampler();

	void create();

	//glm::vec4& pixel(uint64_t row, uint64_t col);
	//const std::vector<glm::vec4>& pixels() const;

	//const glm::vec4& getPixel(uint64_t row, uint64_t col);
	//const glm::vec4& getPixel(float u, float v) const;
	//const glm::vec4& getPixel(glm::vec2 uv) const;

	//void setPixel(glm::vec4& pix, uint64_t row, uint64_t col);
	//void setPixels(std::vector<glm::vec4> pixels, uint64_t width, uint64_t height);
	//void setPixels(void* pixels, uint64_t width, uint64_t height, uint8_t n);

	//bool readFile(std::string file_name);
	//bool writeFile(std::string file_name);
	//bool writeFileHDR(std::string file_name);

	const uint32_t getWidth() const;
	const uint64_t getHeight() const;
	const uint8_t getNbComponents() const;
	//const Image& getImage() const;
};

