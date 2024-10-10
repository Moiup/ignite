#pragma once
#include "Renderer.h"
#include "DefaultRenderer.h"

class RendererDisplayImage : public DefaultRenderer {
private:
	Image* _img{ nullptr };
	uint32_t _width;
	uint32_t _height;

public:
	RendererDisplayImage() = default;

	void render();

	void create();

	void setImage(Image* img);
	void setDim(uint32_t width, uint32_t height);
};