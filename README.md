A game engine based on the Vulkan API.

Currently, it is a prototype.

# Prerequisites
This project works fine on Visual Studio 2022.

# Installation
## Download and configuration
- Download the repository.
- Download the requirements folder (SDL2, GLFW, VulkanSDK, glm) : [click here](https://drive.google.com/file/d/1G_dTxdZMHsAh9MIEBODRKlwur1_edhG3/view?usp=share_link).
- Unzip the `ignite-requirements.zip` folder.
- Inside the folder, you should find another folder also nammed ignite-requirements. Inside this folder, there are 4 other folders : `vulkan_sdk`, `sdl2`, `glfw`, and `glm`.
- Copy these 4 folders inside the project folder in `ignite/IgniteEngine/IgniteEngine`.

## Shader compilation
To compile the required shader for the execution, go to the folder `ignite/IgniteEngine/shaders`, and run `compile.bat`.

Now you should be able to launch the solution `ignite/IgniteEngine/IgniteEngine.sln`, compile and run the programme. You should see three green squares going from the bottom to the top of the window.
