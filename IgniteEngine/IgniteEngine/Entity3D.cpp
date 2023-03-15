#include "Entity3D.h"

Entity3D::Entity3D() :
	_lp{},
	_ap{},
	_lr{},
	_ar{},
	_ls{ 1.0f, 1.0f, 1.0f },
	_as{ 1.0f, 1.0f, 1.0f },
	_parent{ nullptr },
	_children{}
{ ; }

Entity3D* Entity3D::getParent() {
	return _parent;
}

std::vector<Entity3D*>& Entity3D::getChildren() {
	return _children;
}

void Entity3D::addChild(Entity3D* child) {
	_children.push_back(child);
	child->setParent(this);
}

void Entity3D::setPositionAbsolute(float x, float y, float z) {
	setPositionAbsolute(glm::vec3(x, y, z));
}

void Entity3D::setPositionAbsolute(glm::vec3 pos) {
	_lp = _lp + pos - _ap;
	_ap = pos;
}

void Entity3D::setPositionAbsoluteUpdateChildren(float x, float y, float z) {
	setPositionAbsoluteUpdateChildren(glm::vec3(x, y, z));
}

void Entity3D::setPositionAbsoluteUpdateChildren(glm::vec3 pos) {
	glm::vec3 delta = pos - _ap;

	for (Entity3D* child : _children) {
		glm::vec3 n_child_ap = child->getPositionAbsolute() + delta;
		child->setPositionAbsoluteUpdateChildren(n_child_ap);
	}

	setPositionAbsolute(pos);
}

void Entity3D::setPositionLocaleUpdateChildren(float x, float y, float z) {
	setPositionLocaleUpdateChildren(glm::vec3(x, y, z));
}

void Entity3D::setPositionLocaleUpdateChildren(glm::vec3 pos) {
	glm::vec3 delta = pos - _lp;

	for (Entity3D* child : _children) {
		glm::vec3 n_child_lp = child->getPositionLocale() + delta;
		child->setPositionLocaleUpdateChildren(n_child_lp);
	}

	setPositionLocale(pos);
}

void Entity3D::setPositionLocale(float x, float y, float z) {
	setPositionLocale(glm::vec3(x, y, z));
}

void Entity3D::setPositionLocale(glm::vec3 pos) {
	_ap = _ap + pos - _lp;
	_lp = pos;
}

glm::vec3 Entity3D::getPositionAbsolute() {
	return _ap;
}

glm::vec3 Entity3D::getPositionLocale() {
	return _lp;
}

glm::vec3 Entity3D::getScaleAbsolute() {
	return _as;
}

glm::vec3 Entity3D::getScaleLocale() {
	return _ls;
}

glm::mat4 Entity3D::getTranslate() {
	return glm::translate(
		glm::mat4(1.0f),
		_ap
	);
}

void Entity3D::setScaleAbsolute(float sx, float sy, float sz) {
	setScaleAbsolute(glm::vec3(sx, sy, sz));
}

void Entity3D::setScaleAbsolute(glm::vec3 scale) {
	_ls = _ls + scale - _as;
	_as = scale;
}

void Entity3D::setScaleAbsoluteUpdateChildren(float sx, float sy, float sz) {
	setScaleAbsoluteUpdateChildren(glm::vec3(sx, sy, sz));
}

void Entity3D::setScaleAbsoluteUpdateChildren(glm::vec3 scale) {
	glm::vec3 delta = scale - _as;

	for (Entity3D* child : _children) {
		glm::vec3 n_child_as = child->getScaleAbsolute() + delta;
		child->setScaleAbsoluteUpdateChildren(n_child_as);
	}

	setScaleAbsolute(scale);
}

void Entity3D::setScaleLocale(float sx, float sy, float sz) {
	setScaleLocale(glm::vec3(sx, sy, sz));
}

void Entity3D::setScaleLocale(glm::vec3 scale) {
	_as = _as + scale - _ls;
	_ls = scale;
}

void Entity3D::setScaleLocaleUpdateChildren(float sx, float sy, float sz) {
	setScaleLocaleUpdateChildren(glm::vec3(sx, sy, sz));
}

void Entity3D::setScaleLocaleUpdateChildren(glm::vec3 scale) {
	glm::vec3 delta = scale - _ls;

	for (Entity3D* child : _children) {
		glm::vec3 n_child_ls = child->getScaleLocale() + delta;
		child->setScaleLocaleUpdateChildren(n_child_ls);
	}

	setScaleLocale(scale);
}

glm::mat4 Entity3D::getRotate() {
	glm::mat4 r = glm::rotate(
		glm::mat4(1.0f),
		_ar[0],
		glm::vec3(1.0f, 0, 0)
	);

	glm::rotate(
		r,
		_ar[1],
		glm::vec3(0, 1.0f, 0)
	);

	glm::rotate(
		r,
		_ar[2],
		glm::vec3(0, 0, 1.0f)
	);

	return r;
}

glm::mat4 Entity3D::getScale() {
	return glm::scale(
		glm::mat4(1.0f),
		_as
	);
}

glm::mat4 Entity3D::getTransform() {
	return getTranslate() * getRotate() * getScale();
}

void Entity3D::setParent(Entity3D* parent) {
	_parent = parent;
	glm::vec3 locale = _ap - parent->getPositionAbsolute();
	setPositionLocaleUpdateChildren(locale);
}