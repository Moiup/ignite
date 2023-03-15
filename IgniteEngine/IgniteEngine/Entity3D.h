#pragma once

#include "EngineEntity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Entity3D
{
private:
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

	Entity3D* getParent();
	std::vector<Entity3D*>& getChildren();

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

	glm::vec3 getPositionAbsolute();
	glm::vec3 getPositionLocale();

	glm::vec3 getScaleAbsolute();
	glm::vec3 getScaleLocale();

	glm::mat4 getTranslate();
	glm::mat4 getRotate();
	glm::mat4 getScale();

	glm::mat4 getTransform();

private:
	void setParent(Entity3D* parent);
};

