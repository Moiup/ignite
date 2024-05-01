#include "DefaultConf.h"

#include "CameraScene.h"
//#include "PerspectiveCamera.h"

DefaultGPU* DefaultConf::gpu;
LogicalDevice* DefaultConf::logical_device;
GraphicShader* DefaultConf::graphic_shader;
CommandBuffer* DefaultConf::copy_cmd_buf;
WindowSurface* DefaultConf::render_window;
Instance* DefaultConf::instance;
Renderer* DefaultConf::renderer;
CameraScene* DefaultConf::camera;
Queue* DefaultConf::graphics_queue;
Queue* DefaultConf::present_queue;
Queue* DefaultConf::compute_queue;
Texture* DefaultConf::white_texture;
uint64_t DefaultConf::delta_time; // ms
SDL_Event* DefaultConf::event;

VertexBuffer* DefaultConf::coord_buffer;
VertexBuffer* DefaultConf::uv_buffer;
IndexBuffer* DefaultConf::index_buffer;