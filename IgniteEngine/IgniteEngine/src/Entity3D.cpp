#include "Entity3D.h"

std::string makeString(const glm::vec3 v) {
	std::string s = "";
	s += std::to_string(v.x)
		+ " " + std::to_string(v.y)
		+ " " + std::to_string(v.z);
	return s;
}

std::string makeString(const glm::vec4 v) {
	std::string s = "";
	s += std::to_string(v.x)
		+ " " + std::to_string(v.y)
		+ " " + std::to_string(v.z)
		+ " " + std::to_string(v.w);
	return s;
}

std::string makeString(const glm::mat4 m) {
	std::string s = "";
	s = makeString(m, "");
	return s;
}

std::string makeString(const glm::mat4 m, const std::string& tabs) {
	std::string s = "";
	for (int8_t i = 0; i < 4; ++i) {
		s = s + tabs;
		for (int8_t j = 0; j < 4; ++j) {
			s = s + std::to_string(m[j][i]);
			s = s + " ";
		}
		s = s + "\n";
	}
	return s;
}

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

const Entity3D* Entity3D::getParent() const {
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
	for (Entity3D* child : _children) {
		glm::vec3 n_child_ap = pos;
		child->setPositionAbsoluteUpdateChildren(n_child_ap);
	}

	setPositionAbsolute(pos);
}

void Entity3D::setPositionLocaleUpdateChildren(float x, float y, float z) {
	setPositionLocaleUpdateChildren(glm::vec3(x, y, z));
}

void Entity3D::setPositionLocaleUpdateChildren(glm::vec3 pos) {
	setPositionLocale(pos);
	for (Entity3D* child : _children) {
		glm::vec3 n_child_lp = getPositionAbsolute() + pos;
		child->setPositionAbsoluteUpdateChildren(n_child_lp);
	}
}

void Entity3D::setPositionLocale(float x, float y, float z) {
	setPositionLocale(glm::vec3(x, y, z));
}

void Entity3D::setPositionLocale(glm::vec3 pos) {
	//_ap = _ap + pos - _lp;
	_lp = pos;
}

void Entity3D::setRotationAbsolute(float rx, float ry, float rz) {
	setRotationAbsolute(glm::vec3(rx, ry, rz));
}

void Entity3D::setRotationAbsolute(glm::vec3 rot) {
	_ar = rot;
}

void Entity3D::setRotationAbsoluteUpdateChildren(float rx, float ry, float rz) {
	setRotationAbsoluteUpdateChildren(glm::vec3(rx, ry, rz));
}

void Entity3D::setRotationAbsoluteUpdateChildren(glm::vec3 rot) {
	for (Entity3D* child : _children) {
		child->setRotationAbsoluteUpdateChildren(
			rot
		);
	}
	setRotationAbsolute(rot);
}

void Entity3D::setRotationLocale(float rx, float ry, float rz) {
	setRotationLocale(glm::vec3(rx, ry, rz));
}

void Entity3D::setRotationLocale(glm::vec3 rot) {
	_lr = rot;
}

void Entity3D::setRotationLocaleUpdateChildren(float rx, float ry, float rz) {
	setRotationLocaleUpdateChildren(glm::vec3(rx, ry, rz));
}

void Entity3D::setRotationLocaleUpdateChildren(glm::vec3 rot) {
	setRotationLocale(rot);
	for (Entity3D* child : _children) {
		child->setRotationAbsoluteUpdateChildren(
			getRotationAbsolute() + rot
		);
	}
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

glm::mat4 Entity3D::getTranslateLocale() const {
	return glm::translate(
		glm::mat4(1.0f),
		_lp
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


glm::mat4 Entity3D::getRotateLocale() const {
	glm::mat4 r_x = glm::rotate(
		glm::mat4(1.0f),
		_lr.x,
		glm::vec3(1.0f, 0, 0)
	);

	glm::mat4 r_y = glm::rotate(
		glm::mat4(1.0f),
		_lr.y,
		glm::vec3(0, 1.0f, 0)
	);

	glm::mat4 r_z = glm::rotate(
		glm::mat4(1.0f),
		_lr.z,
		glm::vec3(0, 0, 1.0f)
	);

	return r_z * r_y * r_x;
}

glm::mat4 Entity3D::getScale() const {
	return glm::scale(
		glm::mat4(1.0f),
		_ls
	);
}

glm::mat4 Entity3D::getTransformLocale() const {
	return getTranslateLocale() * getRotateLocale() * getScale();
}

glm::mat4 Entity3D::getTransformParent() const {
	glm::mat4 par = glm::mat4(1.0);
	if (_parent) {
		par = _parent->getTransformLocale();
		par = _parent->getTransformParent() * par;
	}

	return par;
}

glm::mat4 Entity3D::getTransform() const {
	glm::mat4 par = glm::mat4(1.0);
	if (_parent) {
		par = _parent->getTransform();
	}

	return par * getTranslateLocale() * getRotateLocale() * getScale();
}

glm::mat4 Entity3D::getTransformWorld() const {
	return _transform_world;
}

void Entity3D::buildTransformWorld() {
	buildTransformWorld(glm::mat4(1.0));
}

void Entity3D::buildTransformWorld(glm::mat4& father_mat) {
	_transform_world = father_mat * getTransformLocale();
	for (Entity3D* child : _children) {
		child->buildTransformWorld(_transform_world);
	}
}

void Entity3D::setParent(Entity3D* parent) {
	_parent = parent;
	//glm::vec3 locale = _ap - parent->getPositionAbsolute();
	//setPositionLocaleUpdateChildren(locale);
}