#pragma once

#include "Device.h"
#include "Image.h"
#include "PhysicalDevice.h"
#include "Pixels.h"
#include "Sampler.h"

#include <cstdint>
#include <iostream>
#include <string>

#include <vector>
#include <glm/glm.hpp>

class Texture2D : public Image
{
private:
	VkSampler _sampler;

public:
	Texture2D() = default;
	Texture2D(
		Device* device,
		uint32_t width,
		uint32_t height,
		IGEImgFormat format = IGEImgFormat::r8g8b8a8_unorm
	);
	Texture2D(
		Device* device,
		uint32_t width,
		uint32_t height,
		Sampler& sampler,
		IGEImgFormat format = IGEImgFormat::r8g8b8a8_unorm
	);

	Texture2D(const Texture2D& tex);
	Texture2D(Texture2D&& tex);
	Texture2D& operator=(const Texture2D& tex);
	Texture2D& operator=(Texture2D&& tex);

	const VkSampler getSampler() const;

	const uint8_t getNbComponents() const;

private:
	void setSampler(Sampler& sampler);
	void destroy();
};

