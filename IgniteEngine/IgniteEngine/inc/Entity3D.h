#pragma once

#include "EngineEntity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class Entity3D
{
protected:
	glm::vec3 _lp;
	glm::vec3 _ap;

	glm::vec3 _lr;
	glm::vec3 _ar;

	glm::vec3 _ls;
	glm::vec3 _as;

	Entity3D* _parent;
	std::vector<Entity3D*> _children;

public:
	Entity3D();
	~Entity3D();

	virtual Entity3D* clone() const = 0;
	virtual Entity3D& operator=(const Entity3D& e);
	virtual void copyAttributes(const Entity3D& e);

	Entity3D* getParent();
	std::vector<Entity3D*>& getChildren();
	const std::vector<Entity3D*>& getChildren() const;

	void addChild(Entity3D* child);

	// Position Absolute
	void setPositionAbsolute(float x, float y, float z);
	void setPositionAbsolute(glm::vec3 pos);
	void setPositionAbsoluteUpdateChildren(float x, float y, float z);
	void setPositionAbsoluteUpdateChildren(glm::vec3 pos);
	// Position Locale
	void setPositionLocale(float x, float y, float z);
	void setPositionLocale(glm::vec3 pos);
	void setPositionLocaleUpdateChildren(float x, float y, float z);
	void setPositionLocaleUpdateChildren(glm::vec3 pos);

	// Rotate Absolute
	void setRotationAbsolute(float rx, float ry, float rz);
	void setRotationAbsolute(glm::vec3 rot);

	// Scale Absolute
	void setScaleAbsolute(float sx, float sy, float sz);
	void setScaleAbsolute(glm::vec3 scale);
	void setScaleAbsoluteUpdateChildren(float sx, float sy, float sz);
	void setScaleAbsoluteUpdateChildren(glm::vec3 scale);
	// Scale Locale
	void setScaleLocale(float sx, float sy, float sz);
	void setScaleLocale(glm::vec3 scale);
	void setScaleLocaleUpdateChildren(float sx, float sy, float sz);
	void setScaleLocaleUpdateChildren(glm::vec3 scale);	

	glm::vec3 getPositionAbsolute() const;
	glm::vec3 getPositionLocale() const;

	glm::vec3 getRotationAbsolute() const;
	glm::vec3 getRotationLocale() const;

	glm::vec3 getScaleAbsolute() const;
	glm::vec3 getScaleLocale() const;

	glm::mat4 getTranslate() const;
	glm::mat4 getRotate() const;
	glm::mat4 getScale() const;

	glm::mat4 getTransform() const;

private:
	void setParent(Entity3D* parent);
};

