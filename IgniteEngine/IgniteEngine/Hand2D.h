#pragma once

#include "Module.h"


class Hand2D : public Module
{
private:
	Mesh _circle_mesh;
	Object3D _circle_obj;

public:
	Hand2D();

	void init();
	void start();
	void update();
	void close();
};