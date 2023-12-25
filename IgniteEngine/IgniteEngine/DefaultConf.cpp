#include "DefaultConf.h"

#include "CameraScene.h"
//#include "PerspectiveCamera.h"

DefaultGPU* DefaultConf::gpu;
LogicalDevice* DefaultConf::logical_device;
GraphicShader* DefaultConf::graphic_shader;
CommandPool* DefaultConf::command_pool;
WindowSurface* DefaultConf::render_window;
Instance* DefaultConf::instance;
Renderer* DefaultConf::renderer;
CameraScene* DefaultConf::camera;
Texture* DefaultConf::white_texture;
uint64_t DefaultConf::delta_time; // ms
SDL_Event* DefaultConf::event;
