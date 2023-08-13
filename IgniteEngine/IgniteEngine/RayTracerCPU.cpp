#include "RayTracerCPU.h"

RayTracerCPU::RayTracerCPU() :
	_nb_cores{1},
	_is_finished{ true }
{
	;
}

int RayTracerCPU::getNbCores() {
	return _nb_cores;
}

void RayTracerCPU::setNbCores(int nb_cores) {
	_nb_cores = nb_cores;
}

bool RayTracerCPU::getIsFinished() {
	return _is_finished;
}

void RayTracerCPU::setIsFinished(bool is_finished) {
	_is_finished = is_finished;
}
