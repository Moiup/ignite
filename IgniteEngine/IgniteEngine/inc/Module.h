#pragma once
#include "EngineEntity.h"
#include "Renderer.h"
#include <vector>
#include "DefaultConf.h"
#include "Camera.h"
#include "Texture.h"
#include "CameraScene.h"
#include "ComputePipeline.h"
#include "Mesh.h"
#include "Object3D.h"
#include "LoadedObjectInfo.h"

class Module : public EngineEntity
{
public:
	static std::vector<Module*> modules;

public:
	Module();

	virtual void init() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void close() = 0;

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();
};

