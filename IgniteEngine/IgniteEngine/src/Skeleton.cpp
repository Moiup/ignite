#include "Skeleton.h"

void Skeleton::setSkeleton(Joint* skeleton) {
	_skeleton = skeleton;
}

const Joint* Skeleton::skeleton() const {
	return _skeleton;
}

std::vector<Joint>& Skeleton::joints() {
	return _joints;
}

//const std::vector<Joint>& Skeleton::joints() const {
//	return _joints;
//}