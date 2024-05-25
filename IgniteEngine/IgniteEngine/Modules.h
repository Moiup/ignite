#pragma once

#include "Window.h"
#include "Renderer.h"
#include "Module.h"
#include "TestModule.h"
#include "MediapipeModule.h"
#include "MenuModule.h"
#include "Camera.h"

#include "Bistro.h"
#include "ComputeShaderTest.h"
#include "ReadWriteImageMod.h"
#include "Hand2D.h"

class Modules
{
private:
	//MenuModule _menu_module;

	//TestModule _test_module;
	//MediapipeModule _mediapipe_module;
	//Bistro _bistro;
	//ComputeShaderTest _compute_shader_test;
	ReadWriteImageMod _read_write_image_mod;
	//Hand2D _hand2d;

public:
	Modules();

	void init();
};

