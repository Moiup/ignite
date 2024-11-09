A game engine based on the Vulkan API.

Currently, it is a prototype.

# Installation
## Prerequisite
The tested operating system where Windows 10 and Windows 11.

This project works fine on Visual Studio 2022. You need a Vulkan ready GPU.

## External
First of all, you will need to download [vulkan](https://vulkan.lunarg.com/sdk/home).
When asked for external packages that come with the sdk, check the SDL library.

## Cloning the repo
This repo uses submodule.

To clone the repo:
```
git clone --recurse-submodules https://github.com/Moiup/ignite.git
```

Or:
```
git clone https://github.com/Moiup/ignite.git
git submodule update --init
```

## Asset folder