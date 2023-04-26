#pragma once

#include "Window.h"
#include "Renderer.h"
#include "Module.h"
#include "TestModule.h"
#include "MediapipeModule.h"
#include "Camera.h"

class Modules
{
private:
	TestModule _test_module;
	//MediapipeModule _mediapipe_module;

public:
	Modules();

	void init();
};

