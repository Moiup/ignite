#include "Skeleton.h"

void Skeleton::setSkeleton(Joint* skeleton) {
	_skeleton = skeleton;
}

const Joint* Skeleton::skeleton() const {
	return _skeleton;
}

Joint* Skeleton::skeleton() {
	return _skeleton;
}

std::vector<Joint>& Skeleton::joints() {
	return _joints;
}

const std::vector<Joint>& Skeleton::joints() const {
	return _joints;
}

std::ostream& operator<<(std::ostream& os, const Skeleton& s){
	os << "skeleton (nb joints = " + std::to_string(s.joints().size()) + "):" << " ";

	os << "[";
	for (const Joint& j : s.joints()) {
		os << " " << std::to_string(j.id());
	}
	os << " ]\n";

	os << s.skeleton()->toString(true);

	return os;
}