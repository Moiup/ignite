#pragma once

#include "Device.h"

#include <iostream>

class Sampler
{
private:
	VkSamplerCreateInfo _info;
	VkSampler _sampler{ nullptr };

	Device* _device{ nullptr };

	int32_t* _shared_count{ nullptr };

public:
	Sampler();
	Sampler(Device& device);
	Sampler(const Sampler& s);
	Sampler(Sampler&& s);
	~Sampler();

	Sampler& operator=(const Sampler& s);
	Sampler& operator=(Sampler&& s);

	const VkSampler getSampler() const;

private:
	void setDevice(Device* device);

	void setPNext(const void* p_next);
	void setFlags(VkSamplerCreateFlags flags);
	void setMagFilter(VkFilter mag_filter);
	void setMinFilter(VkFilter min_filter);
	void setMipmapMode(VkSamplerMipmapMode mipmap_mode);
	void setAddressModeU(VkSamplerAddressMode address_mode_u);
	void setAddressModeV(VkSamplerAddressMode address_mode_v);
	void setAddressModeW(VkSamplerAddressMode address_mode_w);
	void setMipLodBias(float mip_lod_bias);
	void setAnisotropyEnable(VkBool32 anisotropy_enable);
	void setMaxAnisotropy(float max_anisotropy);
	void setCompareEnable(VkBool32 compare_enable);
	void setCompareOp(VkCompareOp compare_op);
	void setMinLod(float min_lod);
	void setMaxLod(float max_lod);
	void setBorderColor(VkBorderColor border_color);
	void setUnnormalizedCoordinates(VkBool32 unnormalized_coordinates);

	void create();
	void destroy();
};

