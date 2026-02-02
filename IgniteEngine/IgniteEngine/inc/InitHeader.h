#pragma once
#include <iostream>
#include <cstdint>

struct InitHeader
{
public:
	uint32_t nb_camera;
	uint32_t type;
	uint32_t nb_repetitions;
	uint32_t nb_points;
};

