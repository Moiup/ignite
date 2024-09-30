#pragma once
#include "Triangle.h"
#include "Texture.h"
#include "DefaultConf.h"
#include "CameraScene.h"

class RayTracerCPU
{
private:
	int _nb_cores;
	bool _is_finished;

	RTScene _scene{};

	Pixels _image;

public:
	RayTracerCPU();

	int getNbCores();
	void setNbCores(int nb_cores);

	bool getIsFinished();
	void setIsFinished(bool is_finished);
	
	void render();

private:
	void buildAssets();
	void buildImage();
	void computePixel(uint64_t x, uint64_t y, glm::mat4 inv);
	void computePixel(float x, float y, uint64_t p_x, uint64_t p_y);
};

