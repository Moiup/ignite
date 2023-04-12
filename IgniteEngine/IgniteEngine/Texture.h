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
	std::vector<glm::vec4> _pixels;
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

public:
	Texture();
	Texture(std::string file_name);
	Texture(std::vector<glm::vec4>& pixels, uint64_t width, uint64_t height);

	void setLogicalDevice(LogicalDevice* _logical_device);
	void setGPU(PhysicalDevice* gpu);
	void setCommandPool(CommandPool* command_pool);

	void create();

	glm::vec4& pixel(uint64_t row, uint64_t col);
	const std::vector<glm::vec4>& pixels() const;

	const glm::vec4& getPixel(uint64_t row, uint64_t col);
	const glm::vec4& getPixel(float u, float v) const;
	const glm::vec4& getPixel(glm::vec2 uv) const;

	void setPixel(glm::vec4& pix, uint64_t row, uint64_t col);
	void setPixels(std::vector<glm::vec4> pixels, uint64_t width, uint64_t height);

	bool readFile(std::string file_name);
	bool writeFile(std::string file_name);

	const uint64_t getWidth() const;
	const uint64_t getHeight() const;
	const Image& getImage() const;
};

