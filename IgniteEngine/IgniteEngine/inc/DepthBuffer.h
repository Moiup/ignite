#pragma once
#include "Image.h"

class DepthBuffer : public Image
{
public:
	DepthBuffer();
	DepthBuffer(
		Device* device,
		uint32_t width,
		uint32_t height,
		const std::vector<uint32_t>& indices
	);


private:
	void setImageQueueFamilyIndices(const std::vector<uint32_t>& indices);
	void setImageInfo();
	void setImageViewInfo();
};

