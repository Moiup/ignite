#pragma once
#include "EngineEntity.h"
#include "Renderer.h"
#include <vector>
#include "DefaultConf.h"
#include "Camera.h"

class Module : public EngineEntity
{
public:
	static std::vector<Module*> modules;

	Renderer* _renderer;
	GraphicShader* _graphics_shader;
	Camera *_camera;
	

public:
	Module();

	void setGraphicsShader(GraphicShader* graphics_shader);
	void setRenderer(Renderer* renderer);
	void setCamera(Camera* camera);

	void init();
	void start();
	void update();
	void close();
};

