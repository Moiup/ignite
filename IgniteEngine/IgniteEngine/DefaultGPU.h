#pragma once
#include "PhysicalDevice.h"

class DefaultGPU: public PhysicalDevice
{
public:
	DefaultGPU();

	void configure(Instance& instance);
};

