#include "DepthBuffer.h"

DepthBuffer::DepthBuffer() :
	Image::Image()
{
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

DepthBuffer::DepthBuffer(
	Device* device,
	uint32_t width,
	uint32_t height,
	const std::vector<uint32_t>& indices
) :
	Image(device, width, height, 1, IGEImgFormat::d32_sfloat_s8_uint)
{
	setImageQueueFamilyIndices(indices);
	setImageInfo();
	setImageViewInfo();
	create();
}

void DepthBuffer::setImageQueueFamilyIndices(const std::vector<uint32_t>& indices) {
	setImageQueueFamilyIndexCount(indices.size());
	setImagePQeueueFamilyIndices(indices.data());
}

void DepthBuffer::setImageInfo() {
	setMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	setImagePNext(nullptr);
	setImageFlags(0);
	setImageImageType(VK_IMAGE_TYPE_2D);
	setImageFormat(VK_FORMAT_D32_SFLOAT_S8_UINT);
	setImageMipLevels(1);
	setImageArrayLayers(1);
	setImageSamples(VK_SAMPLE_COUNT_1_BIT);
	setImageTiling(VK_IMAGE_TILING_OPTIMAL);
	setImageSharingMode(VK_SHARING_MODE_EXCLUSIVE);
	setImageInitialLayout(VK_IMAGE_LAYOUT_UNDEFINED);
	setImageUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
}

void DepthBuffer::setImageViewInfo() {
	setImageViewPNext(nullptr);
	setImageViewFlags(0);
	setImageViewViewType(VK_IMAGE_VIEW_TYPE_2D);
	setImageViewFormat(getImageFormat());
	setImageViewComponents(
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY,
		VK_COMPONENT_SWIZZLE_IDENTITY
	);
	setImageViewSurbresourceRange(
		VK_IMAGE_ASPECT_DEPTH_BIT,
		0,
		1,
		0,
		1
	);
}