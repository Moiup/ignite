#pragma once
#include "Triangle.h"

class RayTracerCPU
{
private:
	int _nb_cores;
	bool _is_finished;

	std::vector<Triangle> _triangles;
	std::vector<Material> _materials;

public:
	RayTracerCPU();

	int getNbCores();
	void setNbCores(int nb_cores);

	bool getIsFinished();
	void setIsFinished(bool is_finished);
	
	void render();

private:
	void buildAssets();
};

