#pragma once
#include "Image2D.h"

class DepthBuffer : public Image2D
{
public:
	DepthBuffer();
	DepthBuffer(
		Device* device,
		uint32_t width,
		uint32_t height,
		std::vector<uint32_t>& indices
	);


private:
	void setImageQueueFamilyIndices(const std::vector<uint32_t>& indices);
	void setImageInfo();
	void setImageViewInfo();
};

