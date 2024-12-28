#pragma once
#include <iostream>
#include <vector>

class EngineEntity
{
public:
	static std::vector<EngineEntity*> engine_entities;

private:
	int32_t* _nb_shared{nullptr};

public:
	EngineEntity();
	EngineEntity(const EngineEntity& e);
	
	~EngineEntity();

	EngineEntity& operator=(const EngineEntity& e);

	int32_t getNbShared();

	virtual void init() = 0;
	virtual void start() = 0;
	virtual void update() = 0;
	virtual void close() = 0;

	static void initAll();
	static void startAll();
	static void updateAll();
	static void closeAll();

protected:
	virtual void destroy();
	virtual void clean();

private:
	void deleteEntity(const EngineEntity& entity);
};

