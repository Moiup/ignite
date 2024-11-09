cmake_minimum_required(VERSION 3.20.0)

project(imgui)

include_directories(
    imgui
    imgui/backends
)

set(HEADERS
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    # imgui/backends/imgui_impl_allegro5.h
    # imgui/backends/imgui_impl_android.h
    # imgui/backends/imgui_impl_dx10.h
    # imgui/backends/imgui_impl_dx11.h
    # imgui/backends/imgui_impl_dx12.h
    # imgui/backends/imgui_impl_dx9.h
    # imgui/backends/imgui_impl_glfw.h
    # imgui/backends/imgui_impl_glut.h
    # imgui/backends/imgui_impl_metal.h
    # imgui/backends/imgui_impl_opengl2.h
    # imgui/backends/imgui_impl_opengl3.h
    # imgui/backends/imgui_impl_opengl3_loader.h
    # imgui/backends/imgui_impl_osx.h
    imgui/backends/imgui_impl_sdl2.h
    # backends/imgui_impl_sdl3.h
    imgui/backends/imgui_impl_sdlrenderer2.h
    # backends/imgui_impl_sdlrenderer3.h
    # backends/imgui_impl_vulkan.h
    # backends/imgui_impl_wgpu.h
    # backends/imgui_impl_win32.h
)

set(SRC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp

    # imgui/backends/imgui_impl_allegro5.cpp
    # imgui/backends/imgui_impl_android.cpp
    # imgui/backends/imgui_impl_dx10.cpp
    # imgui/backends/imgui_impl_dx11.cpp
    # imgui/backends/imgui_impl_dx12.cpp
    # imgui/backends/imgui_impl_dx9.cpp
    # imgui/backends/imgui_impl_glfw.cpp
    # imgui/backends/imgui_impl_glut.cpp
    # imgui/backends/imgui_impl_opengl2.cpp
    # imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    # imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdlrenderer2.cpp
    # imgui/backends/imgui_impl_sdlrenderer3.cpp
    # imgui/backends/imgui_impl_vulkan.cpp
    # imgui/backends/imgui_impl_wgpu.cpp
    # imgui/backends/imgui_impl_win32.cpp
)

add_library(imgui ${SRC} ${HEADERS})

find_package(SDL2 REQUIRED)
target_link_libraries(imgui PRIVATE
    SDL2::SDL2
)


target_include_directories(imgui PUBLIC .)