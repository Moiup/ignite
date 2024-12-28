#include "DefaultConf.h"

#include "CameraScene.h"
//#include "PerspectiveCamera.h"

GraphicsPipelineConfiguration DefaultConf::configuration;
uint32_t DefaultConf::NB_FRAME = 2;
PhysicalDevice* DefaultConf::gpu;
LogicalDevice* DefaultConf::logical_device;
GraphicsPipeline* DefaultConf::graphics_pipeline;
GraphicsPipeline* DefaultConf::debug_pipeline;
CommandBuffer* DefaultConf::copy_cmd_buf;
WindowSurface* DefaultConf::render_window;
uint32_t DefaultConf::render_window_width{ 1600 };
uint32_t DefaultConf::render_window_height{ 900 };
WindowUI* DefaultConf::menu_window;
Instance* DefaultConf::instance;
Renderer* DefaultConf::renderer;
CameraScene* DefaultConf::camera;
Queue* DefaultConf::graphics_queue;
Queue* DefaultConf::present_queue;
Queue* DefaultConf::compute_queue;
Swapchain* DefaultConf::swapchain;
Texture2D* DefaultConf::white_texture;
DepthBuffer* DefaultConf::depth_buffer;
float DefaultConf::delta_time; // ms
SDL_Event* DefaultConf::event;

Buffer<IGEBufferUsage::vertex_buffer>* DefaultConf::coord_buffer;
Buffer<IGEBufferUsage::vertex_buffer>* DefaultConf::uv_buffer;
Buffer<IGEBufferUsage::index_buffer>* DefaultConf::index_buffer;