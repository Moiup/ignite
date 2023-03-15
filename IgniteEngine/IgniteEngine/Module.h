#pragma once
#include "EngineEntity.h"
#include "Renderer.h"
#include <vector>


class Module : public EngineEntity
{
public:
	static std::vector<Module*> modules;

	Renderer* _renderer;
	GraphicShader* _graphics_shader;

public:
	Module();

	void setGraphicsShader(GraphicShader* graphics_shader);
	void setRenderer(Renderer* renderer);

	void init();
	void start();
	void update();
	void close();
};

