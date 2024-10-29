#include "DefaultConf.h"

#include "CameraScene.h"
//#include "PerspectiveCamera.h"

uint32_t DefaultConf::NB_FRAME = 2;
DefaultGPU* DefaultConf::gpu;
LogicalDevice* DefaultConf::logical_device;
GraphicShader* DefaultConf::graphic_shader;
GraphicShader* DefaultConf::debug_shader;
CommandBuffer* DefaultConf::copy_cmd_buf;
WindowSurface* DefaultConf::render_window;
uint32_t DefaultConf::render_window_width;
uint32_t DefaultConf::render_window_height;
WindowUI* DefaultConf::menu_window;
Instance* DefaultConf::instance;
Renderer* DefaultConf::renderer;
CameraScene* DefaultConf::camera;
Queue* DefaultConf::graphics_queue;
Queue* DefaultConf::present_queue;
Queue* DefaultConf::compute_queue;
Texture* DefaultConf::white_texture;
float DefaultConf::delta_time; // ms
SDL_Event* DefaultConf::event;

VertexBuffer* DefaultConf::coord_buffer;
VertexBuffer* DefaultConf::uv_buffer;
IndexBuffer* DefaultConf::index_buffer;