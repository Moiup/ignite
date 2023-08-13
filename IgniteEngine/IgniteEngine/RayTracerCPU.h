#pragma once
class RayTracerCPU
{
private:
	int _nb_cores;
	bool _is_finished;

public:
	RayTracerCPU();

	int getNbCores();
	void setNbCores(int nb_cores);

	bool getIsFinished();
	void setIsFinished(bool is_finished);
};

