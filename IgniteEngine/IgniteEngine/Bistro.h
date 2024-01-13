#pragma once

#include "Module.h"

class Bistro : public Module
{
private:
	LoadedObjectInfo _bistro_info;

	Object3D _bistro;
	bool is_ok{ true };

public:
	Bistro();

	void init();
	void start();
	void update();
	void close();
};

