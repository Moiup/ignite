#pragma once

#include "LogicalDevice.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Buffer.h"
#include "Image.h"
#include "PhysicalDevice.h"

#include <cstdint>
#include <iostream>
#include <string>

#include <vector>
#include <glm/glm.hpp>


class Texture
{
private:
	std::vector<uint8_t> _pixels;
	uint64_t _width;
	uint64_t _height;
	uint8_t _n;
	float _width_inv;
	float _height_inv;

	LogicalDevice* _logical_device;
	PhysicalDevice* _gpu;
	CommandPool* _command_pool;	

	Image _image;
	VkImageLayout _image_layout;

	VkFormat _format;

public:
	Texture();
	Texture(std::string file_name);
	Texture(std::vector<uint8_t>& pixels, uint64_t width, uint64_t height, uint8_t n);

	void setLogicalDevice(LogicalDevice* _logical_device);
	void setGPU(PhysicalDevice* gpu);
	void setCommandPool(CommandPool* command_pool);
	void setFormat(VkFormat format);

	void create();

	//glm::vec4& pixel(uint64_t row, uint64_t col);
	const std::vector<uint8_t>& pixels() const;

	//const glm::vec4& getPixel(uint64_t row, uint64_t col);
	//const glm::vec4& getPixel(float u, float v) const;
	//const glm::vec4& getPixel(glm::vec2 uv) const;

	//void setPixel(glm::vec4& pix, uint64_t row, uint64_t col);
	void setPixels(std::vector<uint8_t> pixels, uint64_t width, uint64_t height, uint8_t n);
	void setPixels(void* pixels, uint64_t width, uint64_t height, uint8_t n);

	bool readFile(std::string file_name);
	bool writeFile(std::string file_name);

	const uint64_t getWidth() const;
	const uint64_t getHeight() const;
	const uint8_t getNbComponents() const;
	const Image& getImage() const;
};

