#include "RendererDisplayImage.h"

void RendererDisplayImage::create() {
	configureQueues();
	createSwapchain();
	createSemaphores();
}

void RendererDisplayImage::setDim(uint32_t width, uint32_t height) {
	_width = width;
	_height = height;
}

void RendererDisplayImage::render() {
	VkResult vk_result{};
	
	_graphics_queues_in_flight[_current_frame].wait();

	vk_result = vkAcquireNextImageKHR(
		(*_graphics_queues)[0].getDevice()->getDevice(),
		_swapchain->getSwapchain(),
		UINT64_MAX,
		_sem_render_starts[_current_frame],
		VK_NULL_HANDLE,
		&_available_img
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed acquiring next image!");
	}

	// --- We want to copy the image into the swapchain image
	// Retreiving the available swapchain image
	Image& swapchain_img = _swapchain->getImages()[_available_img];

	Image& img = *_img;
	_graphics_queues_in_flight[_current_frame].copy(img, swapchain_img);

	// Submit the copy... With a semaphore
	_graphics_queues_in_flight[_available_img].submit(
		1,
		&_sem_render_starts[_current_frame],
		_pipeline_stage_flags.data(),
		1,
		&_sem_render_ends[_current_frame]
	);

	_present_queues_in_flight[_present_frame].present(
		1,
		&_sem_render_ends[_current_frame],
		1,
		&_swapchain->getSwapchain(),
		&_available_img
	);

	_present_frame = _current_frame;
	_current_frame = (_current_frame + 1) % _nb_frame;
}

void RendererDisplayImage::setImage(Image* img) {
	_img = img;
}
