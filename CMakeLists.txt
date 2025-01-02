cmake_minimum_required(VERSION 3.20.0)

project(ignite)

find_package(Vulkan REQUIRED)
set(CMAKE_PREFIX_PATH $ENV{VULKAN_SDK})
set(CMAKE_CXX_STANDARD 17)

set(IGNITE_PATH IgniteEngine/IgniteEngine)

add_subdirectory(${IGNITE_PATH}/libs/fast_obj)
add_subdirectory(${IGNITE_PATH}/libs/imgui)
add_subdirectory(${IGNITE_PATH}/libs/esocket)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(HEADERS
    ${IGNITE_PATH}/inc/App.h
    ${IGNITE_PATH}/inc/Buffer.h
    ${IGNITE_PATH}/inc/Buffer.hpp
    ${IGNITE_PATH}/inc/Camera.h
    ${IGNITE_PATH}/inc/CameraScene.h
    ${IGNITE_PATH}/inc/CommandBuffer.h
    ${IGNITE_PATH}/inc/CommandPool.h
    ${IGNITE_PATH}/inc/ComputePipeline.h
    ${IGNITE_PATH}/inc/ComputeShader.h
    ${IGNITE_PATH}/inc/DefaultConf.h
    ${IGNITE_PATH}/inc/DefaultDispatcher.h
    ${IGNITE_PATH}/inc/DefaultRenderer.h
    ${IGNITE_PATH}/inc/DepthBuffer.h
    ${IGNITE_PATH}/inc/DeviceBuffer.h
    ${IGNITE_PATH}/inc/DeviceBuffer.hpp
    ${IGNITE_PATH}/inc/Device.h
    ${IGNITE_PATH}/inc/Dispatcher.h
    ${IGNITE_PATH}/inc/EngineApp.h
    ${IGNITE_PATH}/inc/EngineEntity.h
    ${IGNITE_PATH}/inc/Entity3D.h
    ${IGNITE_PATH}/inc/GraphicShader.h
    ${IGNITE_PATH}/inc/GraphicsPipeline.h
    ${IGNITE_PATH}/inc/Hand2D.h
    ${IGNITE_PATH}/inc/IGETypes.h
    ${IGNITE_PATH}/inc/Image.h
    ${IGNITE_PATH}/inc/IndexBufferInfo.h
    ${IGNITE_PATH}/inc/InitChildPoint.h
    ${IGNITE_PATH}/inc/InitHeader.h
    ${IGNITE_PATH}/inc/Instance.h
    ${IGNITE_PATH}/inc/Joint.h
    ${IGNITE_PATH}/inc/LoadedObjectInfo.h
    ${IGNITE_PATH}/inc/LogicalDevice.h
    ${IGNITE_PATH}/inc/Material.h
    ${IGNITE_PATH}/inc/Mesh.h
    ${IGNITE_PATH}/inc/Module.h
    ${IGNITE_PATH}/inc/Object3D.h
    ${IGNITE_PATH}/inc/PerspectiveCamera.h
    ${IGNITE_PATH}/inc/PhysicalDevice.h
    ${IGNITE_PATH}/inc/Pipeline.h
    ${IGNITE_PATH}/inc/Pipeline.hpp
    ${IGNITE_PATH}/inc/Pixels.h
    ${IGNITE_PATH}/inc/Pointer.h
    ${IGNITE_PATH}/inc/Pointer.hpp
    ${IGNITE_PATH}/inc/Queue.h
    ${IGNITE_PATH}/inc/ReadWriteImageMod.h
    ${IGNITE_PATH}/inc/Renderer.h
    ${IGNITE_PATH}/inc/RendererDisplayImage.h
    ${IGNITE_PATH}/inc/resource.h
    ${IGNITE_PATH}/inc/Ressource.h
    ${IGNITE_PATH}/inc/Sampler.h
    ${IGNITE_PATH}/inc/Shader.h
    ${IGNITE_PATH}/inc/Skeleton.h
    ${IGNITE_PATH}/inc/SkeletonDebug.h
    ${IGNITE_PATH}/inc/StagingBuffer.h
    ${IGNITE_PATH}/inc/StagingBuffer.hpp
    ${IGNITE_PATH}/inc/Swapchain.h
    ${IGNITE_PATH}/inc/Texture2D.h
    ${IGNITE_PATH}/inc/Window.h
    ${IGNITE_PATH}/inc/WindowSurface.h
    ${IGNITE_PATH}/inc/WindowUI.h
)

set(SRC
    ${IGNITE_PATH}/src/App.cpp
    ${IGNITE_PATH}/src/Camera.cpp
    ${IGNITE_PATH}/src/CameraScene.cpp
    ${IGNITE_PATH}/src/CommandBuffer.cpp
    ${IGNITE_PATH}/src/CommandPool.cpp
    ${IGNITE_PATH}/src/ComputePipeline.cpp
    ${IGNITE_PATH}/src/ComputeShader.cpp
    ${IGNITE_PATH}/src/DefaultConf.cpp
    ${IGNITE_PATH}/src/DefaultDispatcher.cpp
    ${IGNITE_PATH}/src/DefaultRenderer.cpp
    ${IGNITE_PATH}/src/DepthBuffer.cpp
    ${IGNITE_PATH}/src/Device.cpp
    ${IGNITE_PATH}/src/Dispatcher.cpp
    ${IGNITE_PATH}/src/EngineApp.cpp
    ${IGNITE_PATH}/src/EngineEntity.cpp
    ${IGNITE_PATH}/src/Entity3D.cpp
    ${IGNITE_PATH}/src/GraphicShader.cpp
    ${IGNITE_PATH}/src/GraphicsPipeline.cpp
    ${IGNITE_PATH}/src/Hand2D.cpp
    ${IGNITE_PATH}/src/Image.cpp
    ${IGNITE_PATH}/src/IndexBufferInfo.cpp
    ${IGNITE_PATH}/src/InitChildPoint.cpp
    ${IGNITE_PATH}/src/InitHeader.cpp
    ${IGNITE_PATH}/src/Instance.cpp
    ${IGNITE_PATH}/src/Joint.cpp
    ${IGNITE_PATH}/src/LoadedObjectInfo.cpp
    ${IGNITE_PATH}/src/LogicalDevice.cpp
    ${IGNITE_PATH}/src/Material.cpp
    ${IGNITE_PATH}/src/Mesh.cpp
    ${IGNITE_PATH}/src/Module.cpp
    ${IGNITE_PATH}/src/Object3D.cpp
    ${IGNITE_PATH}/src/PerspectiveCamera.cpp
    ${IGNITE_PATH}/src/PhysicalDevice.cpp
    ${IGNITE_PATH}/src/Pipeline.cpp
    ${IGNITE_PATH}/src/Pixels.cpp
    ${IGNITE_PATH}/src/Queue.cpp
    ${IGNITE_PATH}/src/ReadWriteImageMod.cpp
    ${IGNITE_PATH}/src/Renderer.cpp
    ${IGNITE_PATH}/src/RendererDisplayImage.cpp
    ${IGNITE_PATH}/src/Ressource.cpp
    ${IGNITE_PATH}/src/Sampler.cpp
    ${IGNITE_PATH}/src/Shader.cpp
    ${IGNITE_PATH}/src/Skeleton.cpp
    ${IGNITE_PATH}/src/SkeletonDebug.cpp
    ${IGNITE_PATH}/src/Swapchain.cpp
    ${IGNITE_PATH}/src/Texture2D.cpp
    ${IGNITE_PATH}/src/Window.cpp
    ${IGNITE_PATH}/src/WindowSurface.cpp
    ${IGNITE_PATH}/src/WindowUI.cpp
)

set(IG_MAIN_FILES
    ${IGNITE_PATH}/Modules.h
)


file(GLOB_RECURSE IG_MODULES_INC 
    ${IGNITE_PATH}/Modules/inc/*.h
)

file(GLOB_RECURSE IG_MODULES_SRC 
    ${IGNITE_PATH}/Modules/src/*.cpp
)


file(GLOB_RECURSE RAY_TRACING_CPU_INC
    ${IGNITE_PATH}/RayTracingCPU/inc/*.h
)

file(GLOB_RECURSE RAY_TRACING_CPU_SRC
    ${IGNITE_PATH}/RayTracingCPU/src/*.cpp
)

add_library(ignite
    ${SRC}
    ${HEADERS}
    ${IG_MAIN_FILES}
    ${IG_MODULES_INC}
    ${IG_MODULES_SRC}
    ${RAY_TRACING_CPU_INC}
    ${RAY_TRACING_CPU_SRC}
)

source_group("Mains" FILES ${IG_MAIN_FILES})
source_group("Modules/inc" FILES ${IG_MODULES_INC})
source_group("Modules/src" FILES ${IG_MODULES_SRC})
source_group("RayTracingCPU/inc" FILES ${RAY_TRACING_CPU_INC})
source_group("RayTracingCPU/src" FILES ${RAY_TRACING_CPU_SRC})

if(MSVC)
    find_package(SDL2 REQUIRED)
    find_library(LIBS NAMES shaderc_shared)
    target_link_libraries(ignite PRIVATE
        Vulkan::Vulkan
        ${LIBS}
        SDL2::SDL2
        SDL2::SDL2main
        fastObj
        imgui
        ESocket
    )

else()
    find_package(SDL2 REQUIRED)
    target_link_libraries(ignite PRIVATE
        Vulkan::Vulkan
        shaderc_combined
        SDL2::SDL2
        fastObj
        imgui
        ESocket
    )
endif()

target_include_directories(ignite PUBLIC
    ${IGNITE_PATH}/.
    ${IGNITE_PATH}/inc
    ${IGNITE_PATH}/src
    ${IGNITE_PATH}/Modules/inc
    ${IGNITE_PATH}/Modules/src
    ${IGNITE_PATH}/RayTracingCPU/inc
    ${IGNITE_PATH}/RayTracingCPU/src
    ${IGNITE_PATH}/libs/esocket
    ${IGNITE_PATH}/libs/fast_obj
    ${IGNITE_PATH}/libs/glm/glm
    ${IGNITE_PATH}/libs/imgui
    ${IGNITE_PATH}/libs/imgui/imgui
    ${IGNITE_PATH}/libs/stb_image
    ${IGNITE_PATH}/libs/tiny_gltf
    ${Vulkan_INCLUDE_DIR}/SDL2
    ${Vulkan_INCLUDE_DIRS}
)