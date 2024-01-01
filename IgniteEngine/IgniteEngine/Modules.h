#pragma once

#include "Window.h"
#include "Renderer.h"
#include "Module.h"
#include "TestModule.h"
#include "MediapipeModule.h"
#include "MenuModule.h"
#include "Bistro.h"
#include "Camera.h"

class Modules
{
private:
	MenuModule _menu_module;

	//TestModule _test_module;
	//MediapipeModule _mediapipe_module;
	Bistro _bistro;

public:
	Modules();

	void init();
};

