A game engine based on the Vulkan API.

Currently, it is a prototype.

# Installation
## Prerequisite
The tested operating system where Windows 10 and Windows 11.

This project works fine on Visual Studio 2022. You need a Vulkan ready GPU.

## External
First of all, you will need to download [vulkan](https://vulkan.lunarg.com/sdk/home). When asked for external packages that come with the sdk, check the SDL library.

You must have [CMake](https://cmake.org/download/) installed.

## Cloning the repo
This repo uses submodule.

To clone the repo:
```
$ git clone --recurse-submodules https://github.com/Moiup/ignite.git
```

Or:
```
$ git clone https://github.com/Moiup/ignite.git
$ git submodule update --init
```

## Asset folder
To run properly, you must set up the assets.
- Download the asset folder: [click here](https://drive.google.com/file/d/1ReHD4UR76QorcgzefZNy0aDsSDOQzz6U/view?usp=sharing).
- Unzip it.
- Once unziped, you may have an `assets` directory inside `requirements`: `requirements\assets`
- Copy `assets` to `ingite\IgniteEngine`. The final path should be: `ingite\IgniteEngine\assets`

## Generate environment
### Windows (Visual Studio solution)
To generate the environment, run the file:
```
$ IgniteEngine/IgniteEngine/maker.bat
```

Then, run the solution:
```
$ IgniteEngine/IgniteEngine/build/Ignite.sln
```