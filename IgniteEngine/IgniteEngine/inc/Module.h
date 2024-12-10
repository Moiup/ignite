#pragma once
#include "EngineEntity.h"
#include "Renderer.h"
#include <vector>
#include "DefaultConf.h"
#include "Camera.h"
#include "Texture2D.h"
#include "CameraScene.h"
#include "ComputePipeline.h"
#include "Mesh.h"
#include "Object3D.h"
#include "StagingBuffer.h"
#include "DeviceBuffer.h"
#include "LoadedObjectInfo.h"
#include "SkeletonDebug.h"

class Module : public EngineEntity
{
public:
	static std::vector<Module*> modules;

public:
	Module();

	virtual void init() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	void close() = 0;

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();
};

