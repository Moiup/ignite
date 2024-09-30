#pragma once
#include <iostream>
#include <vector>

class EngineEntity
{
public:
	static std::vector<EngineEntity*> engine_entities;

public:
	EngineEntity();

	virtual void init() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void close() = 0;

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();
};

