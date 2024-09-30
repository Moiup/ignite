#ifndef SKELETON_H
#define SKELETON_H

#include "Joint.h"


class Skeleton {
private:
	Joint* _skeleton;
	std::vector<Joint> _joints;

public:
	Skeleton() = default;

	void setSkeleton(Joint* skeleton);
	const Joint* skeleton() const;

	std::vector<Joint>& joints();
	//const std::vector<Joint>& joints() const;

	void duplicate(const Skeleton& skeleton);
};

#endif