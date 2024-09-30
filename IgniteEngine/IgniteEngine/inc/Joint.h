#ifndef JOINT_H
#define JOINT_H

#include "Entity3D.h"

class Joint : public Entity3D {
private:
	glm::mat4 _inverse_bind_matrices;
	uint32_t _id;

public:
	Joint();

	Entity3D* clone() const;
	Joint& operator=(const Joint& j);
	void copyAttributes(const Joint& j);

	glm::mat4& inverseBindMatrices();
	const glm::mat4& inverseBindMatrices() const;

	void setId(uint32_t id);
	int32_t id() const;
};

#endif