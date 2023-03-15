#pragma once
#include "Image.h"

class DepthBuffer : public Image
{
public:
	DepthBuffer();

	void create();
	void destroy();

	void setImageWidthHeight(uint32_t width, uint32_t height);
	void setImageQueueFamilyIndices(const std::vector<uint32_t>& indices);

private:
	void setImageInfo();
	void setImageViewInfo();
};

