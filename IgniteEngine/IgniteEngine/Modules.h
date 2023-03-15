#pragma once

#include "Window.h"
#include "Renderer.h"
#include "Module.h"
#include "TestModule.h"
#include "MediapipeModule.h"

class Modules
{
private:
	GraphicShader* _graphics_shader;
	Renderer* _renderer;

	TestModule _test_module;
	//MediapipeModule _mediapipe_module;

public:
	Modules();

	void init();

	void setGraphicsShader(GraphicShader* graphics_shader);
	void setRenderer(Renderer* renderer);
};

