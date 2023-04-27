#include "DefaultConf.h"

#include "PerspectiveCamera.h"

DefaultGPU* DefaultConf::gpu;
LogicalDevice* DefaultConf::logical_device;
GraphicShader* DefaultConf::graphic_shader;
CommandPool* DefaultConf::command_pool;
WindowSurface* DefaultConf::render_window;
Renderer* DefaultConf::renderer;
CameraScene* DefaultConf::camera;
uint64_t DefaultConf::delta_time; // ms
SDL_Event* DefaultConf::event;