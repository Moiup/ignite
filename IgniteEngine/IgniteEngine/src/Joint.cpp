#include "Joint.h"

Joint::Joint() :
	_inverse_bind_matrices{glm::mat4(1.0f)}
{ ; }

Entity3D* Joint::clone() const {
	Joint* j = new Joint;

	j->copyAttributes(*this);

	for (Entity3D* child : this->_children) {
		Joint* child_j = reinterpret_cast<Joint*>(child);
		j->addChild(child_j->clone());
	}

	return j;
}

Joint& Joint::operator=(const Joint& j) {
	copyAttributes(j);

	for (Entity3D* child : j._children) {
		Joint* child_j = reinterpret_cast<Joint*>(child);
		this->addChild(child_j->clone());
	}

	return *this;
}

void Joint::copyAttributes(const Joint& j) {
	Entity3D::copyAttributes(j);
	this->_inverse_bind_matrices = j._inverse_bind_matrices;
	this->_id = j._id;
}

glm::mat4& Joint::inverseBindMatrices() {
	return _inverse_bind_matrices;
}

const glm::mat4& Joint::inverseBindMatrices() const {
	return _inverse_bind_matrices;
}

void Joint::setId(uint32_t id) {
	_id = id;
}

int32_t Joint::id() const {
	return _id;
}
