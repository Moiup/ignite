#pragma once

#include "Module.h"



class Hand2D : public Module
{
private:
	Mesh _circle_mesh;
	Object3D _circle_obj;

	bool _modify_vertex{false};
	bool _modify_buffer{ false };

	Pixels _hand_img;

	uint32_t _center_x;
	uint32_t _center_y;
	float _pix_to_world = 0.01;
	float _world_to_pix = 100;

public:
	Hand2D();

	void init();
	void start();
	void update();
	void close();
};