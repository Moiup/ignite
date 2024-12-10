//#pragma once
//#include "imgui/imgui.h"
//#include "imgui/backends/imgui_impl_sdl2.h"
//#include "imgui/backends/imgui_impl_sdlrenderer2.h"
//
//#include "Module.h"
//#include "DefaultDispatcher.h"
//#include "RendererDisplayImage.h"
//
//struct GreenScreenPC {
//    glm::vec4 _color;
//    uint32_t _width;
//    uint32_t _height;
//    float _hardness;
//};
//
//class GreenScreen : public Module
//{
//private:
//    Texture _input;
//    Texture _output;
//
//    GreenScreenPC _push_constant;
//
//    ComputeShader _green_screen_comp;
//    ComputePipeline _compute_pipeline;
//    DefaultDispatcher _dispatcher;
//
//    Pixels _img;
//
//    WindowSurface _display_window{};
//    RendererDisplayImage _rdi;
//
//public:
//    GreenScreen() = default;
//
//    void init();
//    void start();
//    void update();
//    void close();
//
//private:
//    void createShader();
//};
//
