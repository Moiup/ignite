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
	Joint* skeleton();

	std::vector<Joint>& joints();
	const std::vector<Joint>& joints() const;

	friend std::ostream& operator<<(std::ostream& os, const Skeleton& s);
};

#endif