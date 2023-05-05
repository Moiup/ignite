#pragma once
#include "EngineEntity.h"
#include "Renderer.h"
#include <vector>
#include "DefaultConf.h"
#include "Camera.h"
#include "Texture.h"
#include "CameraScene.h"

class Module : public EngineEntity
{
public:
	static std::vector<Module*> modules;

public:
	Module();

	void init();
	void start();
	void update();
	void close();

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();
};

