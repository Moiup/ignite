#include "Joint.h"

Joint::Joint() :
	_inverse_bind_matrices{glm::mat4(1.0f)}
{ ; }

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
