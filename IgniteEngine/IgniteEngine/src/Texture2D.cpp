#include "Texture2D.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//
//#include "stb_image/stb_image.h"
//#include "stb_image/stb_image_write.h"


Texture2D::Texture2D(
	Device* device,
	uint32_t width,
	uint32_t height,
	IGEImgFormat format
) :
	Image(device, width, height, 1, format)
{
	setImageImageType(VK_IMAGE_TYPE_2D);
	setImageViewViewType(VK_IMAGE_VIEW_TYPE_2D);
	create();
}


Texture2D::Texture2D(
	Device* device,
	uint32_t width,
	uint32_t height,
	Sampler& sampler,
	IGEImgFormat format
) :
	Texture2D(device, width, height, format)
{
	setSampler(sampler);
}


Texture2D::Texture2D(const Texture2D& tex) {
	*this = tex;
}


Texture2D& Texture2D::operator=(const Texture2D& tex) {
	Image::operator=(tex);
	_sampler = tex._sampler;

	return *this;
}

void Texture2D::setSampler(Sampler& sampler) {
	_sampler = sampler.getSampler();
}


VkSampler Texture2D::getSampler() {
	return _sampler;
}

const uint8_t Texture2D::getNbComponents() const {
	return _n;
}
