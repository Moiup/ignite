#pragma once

#include "LogicalDevice.h"
#include "DefaultGPU.h"
#include "GraphicShader.h"

class DefaultConf
{
public:
	static DefaultGPU *gpu;
	static LogicalDevice *logical_device;
	static GraphicShader *graphic_shader;
};

