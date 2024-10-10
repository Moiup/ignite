#ifndef JOINT_H
#define JOINT_H

#include "Entity3D.h"
#include <iostream>

class Joint : public Entity3D {
private:
	glm::mat4 _inverse_bind_matrices;
	glm::mat4 _initial_transform;
	uint32_t _id;
	std::string _name;

public:
	Joint();

	Entity3D* clone() const;
	Joint& operator=(const Joint& j);
	void copyAttributes(const Joint& j);

	glm::mat4& inverseBindMatrices();
	const glm::mat4& inverseBindMatrices() const;

	glm::mat4& initialTransform();
	const glm::mat4& initialTransform() const;

	void setId(uint32_t id);
	int32_t id() const;

	std::string& name();
	const std::string& name() const;

	std::string toString(bool is_matrix = false) const;

	friend std::ostream& operator<<(std::ostream& os, const Joint& j);

private:
	void toStringChild(std::string& s, std::string tabs, int nb_tab, bool is_matrix) const;
};

#endif