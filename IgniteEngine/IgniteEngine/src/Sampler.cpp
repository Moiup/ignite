#include "Sampler.h"

Sampler::Sampler() :
	_info{},
	_sampler{nullptr}
{
	_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	_info.magFilter = VK_FILTER_LINEAR;
	_info.minFilter = VK_FILTER_LINEAR;
	_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_info.mipLodBias = 0.0f;
	_info.anisotropyEnable = VK_FALSE;
	_info.maxAnisotropy = 1.0f;
	_info.compareEnable = VK_FALSE;
	_info.compareOp = VK_COMPARE_OP_NEVER;
	_info.minLod = 0.0f;
	_info.maxLod = 0.0f;
	_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	_info.unnormalizedCoordinates = VK_FALSE;

	_shared_count = new int32_t(1);
}

Sampler::Sampler(Device& device) :
	Sampler::Sampler()
{
	_device = &device;
	create();
}

Sampler::Sampler(const Sampler& s) {
	*this = s;
}

Sampler::Sampler(Sampler&& s) {
	*this = std::move(s);
}

Sampler::~Sampler() {
	destroy();
}

Sampler& Sampler::operator=(const Sampler& s) {
	destroy();

	_info = s._info;
	_sampler = s._sampler;
	_device = s._device;
	_shared_count = s._shared_count;
	*_shared_count += 1;

	return *this;
}

Sampler& Sampler::operator=(Sampler&& s) {
	destroy();
	
	_info = std::move(s)._info;
	_sampler = std::move(s)._sampler;
	s._sampler = nullptr;
	_device = std::move(s)._device;
	s._device = nullptr;
	
	_shared_count = std::move(s)._shared_count;
	s._shared_count = nullptr;

	return *this;
}

void Sampler::create() {
	VkResult result = vkCreateSampler(
		_device->getDevice(),
		&_info,
		nullptr,
		&_sampler
	);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Error creating the sampler.");
	}
}

void Sampler::destroy() {
	if (!_shared_count) {
		return;
	}

	*_shared_count -= 1;
	if (*_shared_count) {
		return;
	}

	delete _shared_count;
	_shared_count = nullptr;

	if (!_sampler) {
		return;
	}
	vkDestroySampler(
		_device->getDevice(),
		_sampler,
		nullptr
	);
}

const VkSampler Sampler::getSampler() const {
	return _sampler;
}

void Sampler::setDevice(Device* device) {
	_device = device;
}

void Sampler::setPNext(const void* p_next) {
	_info.pNext = p_next;
}

void Sampler::setFlags(VkSamplerCreateFlags flags) {
	_info.flags = flags;
}

void Sampler::setMagFilter(VkFilter mag_filter) {
	_info.magFilter = mag_filter;
}

void Sampler::setMinFilter(VkFilter min_filter) {
	_info.minFilter = min_filter;
}

void Sampler::setMipmapMode(VkSamplerMipmapMode mipmap_mode) {
	_info.mipmapMode = mipmap_mode;
}

void Sampler::setAddressModeU(VkSamplerAddressMode address_mode_u) {
	_info.addressModeU = address_mode_u;
}

void Sampler::setAddressModeV(VkSamplerAddressMode address_mode_v) {
	_info.addressModeV = address_mode_v;
}

void Sampler::setAddressModeW(VkSamplerAddressMode address_mode_w) {
	_info.addressModeW = address_mode_w;
}

void Sampler::setMipLodBias(float mip_lod_bias) {
	_info.mipLodBias = mip_lod_bias;
}

void Sampler::setAnisotropyEnable(VkBool32 anisotropy_enable) {
	_info.anisotropyEnable = anisotropy_enable;
}

void Sampler::setMaxAnisotropy(float max_anisotropy) {
	_info.maxAnisotropy = max_anisotropy;
}

void Sampler::setCompareEnable(VkBool32 compare_enable) {
	_info.compareEnable = compare_enable;
}

void Sampler::setCompareOp(VkCompareOp compare_op) {
	_info.compareOp = compare_op;
}

void Sampler::setMinLod(float min_lod) {
	_info.minLod = min_lod;
}

void Sampler::setMaxLod(float max_lod) {
	_info.maxLod = max_lod;
}

void Sampler::setBorderColor(VkBorderColor border_color) {
	_info.borderColor = border_color;
}

void Sampler::setUnnormalizedCoordinates(VkBool32 unnormalized_coordinates) {
	_info.unnormalizedCoordinates = unnormalized_coordinates;
}