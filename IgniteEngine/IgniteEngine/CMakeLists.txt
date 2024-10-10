cmake_minimum_required(VERSION 3.20.0)

project(Ignite)

find_package(Vulkan REQUIRED)
set(CMAKE_PREFIX_PATH $ENV{VULKAN_SDK})


add_subdirectory(libs/fast_obj)
add_subdirectory(libs/imgui)
# add_subdirectory(libs/)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include_directories(
    .
    inc
    src
    RayTracingCPU/inc
    RayTracingCPU/src
    Modules/inc
    Modules/src
    libs
    libs/imgui
    libs/stb_image
    libs/json
    libs/tiny_gltf
    ${Vulkan_INCLUDE_DIR}/SDL2
)

set(HEADERS    
    inc/App.h
    inc/ArrayBuffer.h
    inc/ArrayBufferInfo.h
    inc/Buffer.h
    inc/Camera.h
    inc/CameraScene.h
    inc/CommandBuffer.h
    inc/CommandPool.h
    inc/ComputePipeline.h
    inc/ComputeShader.h
    inc/ComputeShaderTest.h
    inc/DefaultConf.h
    inc/DefaultDispatcher.h
    inc/DefaultGPU.h
    inc/DefaultRenderer.h
    inc/DepthBuffer.h
    inc/DeviceBuffer.h
    inc/Device.h
    inc/Dispatcher.h
    inc/EngineApp.h
    inc/EngineEntity.h
    inc/Entity3D.h
    inc/GraphicShader.h
    inc/GraphicsPipeline.h
    inc/Hand2D.h
    # inc/HeaderMessageProtocoleNetwork.h
    inc/Image.h
    inc/ImageRessource.h
    inc/IndexBuffer.h
    inc/IndexBufferInfo.h
    inc/InitChildPoint.h
    inc/InitHeader.h
    # inc/InitNetworkProtocole.h
    inc/Instance.h
    inc/Joint.h
    inc/LoadedObjectInfo.h
    inc/LogicalDevice.h
    inc/Material.h
    inc/Mesh.h
    # inc/MessageProtocoleNetwork.h
    # inc/MocapMessageProtocolNetwork.h
    inc/Module.h
    inc/Object3D.h
    inc/PerspectiveCamera.h
    inc/PhysicalDevice.h
    inc/Pipeline.h
    inc/Pixels.h
    inc/Pointer.h
    inc/Pointer.hpp
    inc/PushConstantInfo.h
    inc/Queue.h
    inc/ReadWriteImageMod.h
    inc/Renderer.h
    inc/RendererDisplayImage.h
    inc/resource.h
    inc/Ressource.h
    inc/Sampler.h
    inc/SamplerInfo.h
    # inc/SecondaryMocapHeaderNetwork.h
    inc/Shader.h
    inc/Skeleton.h
    inc/SkeletonDebug.h
    inc/StagingBuffer.h
    inc/StorageBuffer.h
    inc/Swapchain.h
    inc/Texture.h
    inc/TextureInfo.h
    inc/UniformBuffer.h
    inc/VertexBuffer.h
    inc/VertexBufferInfo.h
    inc/Window.h
    inc/WindowSurface.h
    inc/WindowUI.h
)

set(SRC
    src/App.cpp
    src/ArrayBuffer.cpp
    src/ArrayBufferInfo.cpp
    src/Buffer.cpp
    src/Camera.cpp
    src/CameraScene.cpp
    src/CommandBuffer.cpp
    src/CommandPool.cpp
    src/ComputePipeline.cpp
    src/ComputeShader.cpp
    src/ComputeShaderTest.cpp
    src/DefaultConf.cpp
    src/DefaultDispatcher.cpp
    src/DefaultGPU.cpp
    src/DefaultRenderer.cpp
    src/DepthBuffer.cpp
    src/DeviceBuffer.cpp
    src/Device.cpp
    src/Dispatcher.cpp
    src/EngineApp.cpp
    src/EngineEntity.cpp
    src/Entity3D.cpp
    src/GraphicShader.cpp
    src/GraphicsPipeline.cpp
    src/Hand2D.cpp
    # src/HeaderMessageProtocoleNetwork.cpp
    src/Image.cpp
    src/ImageRessource.cpp
    src/IndexBuffer.cpp
    src/IndexBufferInfo.cpp
    src/InitChildPoint.cpp
    src/InitHeader.cpp
    # src/InitNetworkProtocole.cpp
    src/Instance.cpp
    src/Joint.cpp
    src/LoadedObjectInfo.cpp
    src/LogicalDevice.cpp
    src/Material.cpp
    src/Mesh.cpp
    # src/MessageProtocoleNetwork.cpp
    # src/MocapMessageProtocolNetwork.cpp
    src/Module.cpp
    src/Object3D.cpp
    src/PerspectiveCamera.cpp
    src/PhysicalDevice.cpp
    src/Pipeline.cpp
    src/Pixels.cpp
    src/PushConstantInfo.cpp
    src/Queue.cpp
    src/ReadWriteImageMod.cpp
    src/Renderer.cpp
    src/RendererDisplayImage.cpp
    src/Ressource.cpp
    src/Sampler.cpp
    src/SamplerInfo.cpp
    # src/SecondaryMocapHeaderNetwork.cpp
    src/Shader.cpp
    src/Skeleton.cpp
    src/SkeletonDebug.cpp
    src/StagingBuffer.cpp
    src/StorageBuffer.cpp
    src/Swapchain.cpp
    src/Texture.cpp
    src/TextureInfo.cpp
    src/UniformBuffer.cpp
    src/VertexBuffer.cpp
    src/VertexBufferInfo.cpp
    src/Window.cpp
    src/WindowSurface.cpp
    src/WindowUI.cpp
)

set(IG_MAIN_FILES
    IgniteEngine.cpp
    Modules.cpp
    Modules.h
)

file(GLOB_RECURSE IG_MODULES_INC 
    Modules/inc/*.h
)

file(GLOB_RECURSE IG_MODULES_SRC 
    Modules/src/*.cpp
)

file(GLOB_RECURSE RAY_TRACING_CPU_INC
    RayTracingCPU/inc/*.h
)

file(GLOB_RECURSE RAY_TRACING_CPU_SRC
    RayTracingCPU/src/*.cpp
)

add_executable(Ignite
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
    target_link_libraries(Ignite PRIVATE
        Vulkan::Vulkan
        ${LIBS}
        SDL2::SDL2
        SDL2::SDL2main
        fastObj
        imgui
    )

else()
    find_package(SDL2 REQUIRED)
    target_link_libraries(Ignite PRIVATE
        Vulkan::Vulkan
        shaderc_combined
        SDL2::SDL2
        fastObj
        imgui
    )
endif()

# target_link_libraries(Ignite PRIVATE SDL2::SDL2

# target_link_libraries(Ignite fast_obj)