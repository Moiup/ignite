#include "Hit.h"


Hit::Hit() :
	_t{Hit::inf()}
{
	;
}

Hit::Hit(float t) :
	_t{ t }
{
	;
}

const float Hit::t() const {
	return _t;
}

float& Hit::t() {
	return _t;
}

Hit::operator bool() const {
	return _t < Hit::inf();
}

bool Hit::operator>(Hit& h) {
	return _t > h.t();
}

bool Hit::operator<(Hit& h) {
	return _t < h.t();
}

const float Hit::inf() {
	return std::numeric_limits<float>::infinity();
}