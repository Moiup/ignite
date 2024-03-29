#pragma once
#include <iostream>
#include <vector>

class EngineEntity
{
public:
	static std::vector<EngineEntity*> engine_entities;

public:
	EngineEntity();

	virtual void init();
	virtual void start();
	virtual void update();
	virtual void close();

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();
};

