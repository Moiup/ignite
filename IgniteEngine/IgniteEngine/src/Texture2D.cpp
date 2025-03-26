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
	Image(
		device,
		width,
		height,
		1,
		format,
		(VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT |
		VK_IMAGE_USAGE_STORAGE_BIT
		),
		VK_IMAGE_TYPE_2D
	)
{
	//setImageImageType(VK_IMAGE_TYPE_2D);
	//setImageViewViewType(VK_IMAGE_VIEW_TYPE_2D);
	//create();
	;
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

Texture2D::Texture2D(Texture2D&& tex) {
	*this = std::move(tex);
}

Texture2D& Texture2D::operator=(const Texture2D& tex) {
	destroy();
	Image::operator=(tex);
	_sampler = tex._sampler;

	return *this;
}

Texture2D& Texture2D::operator=(Texture2D&& tex) {
	destroy();
	Image::operator=(std::move(tex));
	_sampler = std::move(tex)._sampler;
	tex._sampler = nullptr;

	return *this;
}

void Texture2D::setSampler(Sampler& sampler) {
	_sampler = sampler.getSampler();
}

const VkSampler Texture2D::getSampler() const {
	return _sampler;
}

const uint8_t Texture2D::getNbComponents() const {
	return _n;
}

void Texture2D::destroy() {
	_sampler = nullptr;
}