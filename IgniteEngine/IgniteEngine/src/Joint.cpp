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

glm::mat4& Joint::initialTransform() {
	return _initial_transform;
}

const glm::mat4& Joint::initialTransform() const {
	return _initial_transform;
}

void Joint::setId(uint32_t id) {
	_id = id;
}

int32_t Joint::id() const {
	return _id;
}

std::string& Joint::name() {
	return _name;
}

const std::string& Joint::name() const {
	return _name;
}

std::string Joint::toString(bool is_matrix) const {
	std::string s = "";
	std::string tabs = "";

	toStringChild(s, tabs, 4, is_matrix);

	return s;
}

std::ostream& operator<<(std::ostream& os, const Joint& j) {
	os << j._id << " " << j._name;
	return os;
}

void Joint::toStringChild(std::string& s, std::string tabs, int nb_tab, bool is_matrix) const {
	for (int32_t i = 0; i < nb_tab; ++i) {
		tabs = tabs + " ";
	}
	s = s + tabs + "==== joint: " + std::to_string(_id) + " - " + _name + " ====\n";
	s = s + tabs + "children [ ";
	for (Entity3D* c : _children) {
		Joint* j = reinterpret_cast<Joint*>(c);
		s = s + std::to_string(j->id()) + " ";
	}
	s = s + "]\n";

	if (is_matrix) {
		s = s + tabs + "  transform locale:\n";
		s = s + makeString(getTransformLocale(), tabs);
		s = s + tabs + "  inverse bind matrix:\n";
		s = s + makeString(inverseBindMatrices(), tabs);
		s = s + tabs + "  T * IBM:\n";
		s = s + makeString(getTransform() * inverseBindMatrices(), tabs);
	}

	for (Entity3D* c : _children) {
		Joint* j = reinterpret_cast<Joint*>(c);
		j->toStringChild(s, tabs, 4, is_matrix);
	}
}