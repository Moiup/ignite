#include "Renderer.h"

Renderer::Renderer() :
	_nb_frame{ 2 },
	_logical_device{ nullptr },
	_command_pool{ nullptr },
	_gpu{ nullptr },
	_window{nullptr},
	_clear_color_value{0.2f, 0.2f, 0.2f, 1.0f},
	_command_buffers{},
	_current_frame{0},
	_pipeline_stage_flags{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}
{ ; }

void Renderer::create() {
	;
}

void Renderer::destroy() {
	;
}

void Renderer::render() {
	;
}

void Renderer::setGraphicsQueues(std::vector<Queue>* graphics_queues) {
	_graphics_queues = graphics_queues;
}

void Renderer::setPresentQueues(std::vector<Queue>* present_queues) {
	_present_queues = present_queues;
}

void Renderer::setNbFrame(uint32_t nb_frame) {
	_nb_frame = nb_frame;
}

void Renderer::setPhysicalDevice(PhysicalDevice* gpu) {
	_gpu = gpu;
}

void Renderer::setWindow(WindowSurface* window) {
	_window = window;
}

void Renderer::setClearColorValue(VkClearColorValue clear_color_value) {
	_clear_color_value = clear_color_value;
}

void Renderer::createFencesAndSemaphores() {
	;
}

void Renderer::createSwapchain() {
	;
}

void Renderer::createDepthBuffer() {
	;
}

void Renderer::createGraphicsPipeline() {
	;
}

void Renderer::createCommandBuffers() {
	;
}