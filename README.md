A game engine based on the Vulkan API.

Currently, it is a prototype.

# Installation
## Prerequisite
The tested operating system where Windows 10 and Windows 11.

This project works fine on Visual Studio 2022. You need a Vulkan ready GPU.

## Downloads
- Download the repository.
- Download the requirements folder (SDL2, GLFW, VulkanSDK, glm) : [click here](https://drive.google.com/file/d/1G_dTxdZMHsAh9MIEBODRKlwur1_edhG3/view?usp=share_link).
- Unzip the `ignite-requirements.zip` folder.
- Inside the folder, you should find another folder also nammed ignite-requirements. Inside this folder, there are 4 other folders : `vulkan_sdk`, `sdl2`, `glfw`, and `glm`.
- Copy these 4 folders inside the project folder in `ignite/IgniteEngine/IgniteEngine`.

## Shader compilation
To compile the required shaders for the execution, go to the folder `ignite/IgniteEngine/shaders`, and run `compile.bat` and `compile_red.bat`.

## Compile and run
Launch the solution `ignite/IgniteEngine/IgniteEngine.sln`.

The compilation should be configured in `Debug` or `Release` mode, and the plateforme set to `x64`. Then you can compile and run the programme. You should see three squares going from the bottom to the top of the window. The one in the middle is green, the others are red.

# Code
The entry point (main file) is `IgniteEngine.cpp`.