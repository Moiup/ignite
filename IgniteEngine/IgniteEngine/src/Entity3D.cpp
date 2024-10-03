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

Entity3D::~Entity3D() {
	for (Entity3D* child : _children) {
		delete child;
	}
}

Entity3D& Entity3D::operator=(const Entity3D& e) {
	copyAttributes(e);

	for (Entity3D* child : e._children) {
		this->addChild(child->clone());
	}

	return *this;
}

void Entity3D::copyAttributes(const Entity3D& e) {
	this->_parent = e._parent;
	this->_lp = e._lp;
	this->_ap = e._ap;
	this->_lr = e._lr;
	this->_ar = e._ar;
	this->_ls = e._ls;
	this->_as = e._as;
}

Entity3D* Entity3D::getParent() {
	return _parent;
}

std::vector<Entity3D*>& Entity3D::getChildren() {
	return _children;
}

const std::vector<Entity3D*>& Entity3D::getChildren() const {
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
	_ap = pos;
}

void Entity3D::setPositionAbsoluteUpdateChildren(float x, float y, float z) {
	setPositionAbsoluteUpdateChildren(glm::vec3(x, y, z));
}

void Entity3D::setPositionAbsoluteUpdateChildren(glm::vec3 pos) {
	//glm::vec3 delta = pos - _ap;

	for (Entity3D* child : _children) {
		glm::vec3 n_child_ap = child->getPositionLocale() + pos;
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

void Entity3D::setRotationAbsolute(float rx, float ry, float rz) {
	setRotationAbsolute(glm::vec3(rx, ry, rz));
}

void Entity3D::setRotationAbsolute(glm::vec3 rot) {
	_lr = _lr + rot - _ar;
	_ar = rot;
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

glm::vec3 Entity3D::getPositionAbsolute() const {
	return _ap;
}

glm::vec3 Entity3D::getPositionLocale() const {
	return _lp;
}

glm::vec3 Entity3D::getRotationAbsolute() const {
	return _ar;
}

glm::vec3 Entity3D::getRotationLocale() const {
	return _lr;
}

glm::vec3 Entity3D::getScaleAbsolute() const {
	return _as;
}

glm::vec3 Entity3D::getScaleLocale() const {
	return _ls;
}

glm::mat4 Entity3D::getTranslate() const {
	return glm::translate(
		glm::mat4(1.0f),
		_ap
	);
}

glm::mat4 Entity3D::getRotate() const {
	glm::mat4 r_x = glm::rotate(
		glm::mat4(1.0f),
		_ar.x,
		glm::vec3(1.0f, 0, 0)
	);

	glm::mat4 r_y = glm::rotate(
		glm::mat4(1.0f),
		_ar.y,
		glm::vec3(0, 1.0f, 0)
	);

	glm::mat4 r_z = glm::rotate(
		glm::mat4(1.0f),
		_ar.z,
		glm::vec3(0, 0, 1.0f)
	);

	return r_x * r_y * r_z;
}

glm::mat4 Entity3D::getScale() const {
	return glm::scale(
		glm::mat4(1.0f),
		_as
	);
}

glm::mat4 Entity3D::getTransform() const {
	return getTranslate() * getRotate() * getScale();
}

void Entity3D::setParent(Entity3D* parent) {
	_parent = parent;
	glm::vec3 locale = _ap - parent->getPositionAbsolute();
	setPositionLocaleUpdateChildren(locale);
}