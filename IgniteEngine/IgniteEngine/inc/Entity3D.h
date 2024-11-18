#pragma once

#include "EngineEntity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include <iostream>

std::string makeString(const glm::vec3 m);
std::string makeString(const glm::vec4 m);
std::string makeString(const glm::mat4 m);
std::string makeString(const glm::mat4 m, const std::string& tabs);

class Entity3D
{
protected:
	glm::vec3 _lp;
	glm::vec3 _ap;

	glm::vec3 _lr;
	glm::vec3 _ar;

	glm::vec3 _ls;
	glm::vec3 _as;

	glm::mat4 _transform_world;

	Entity3D* _parent;
	std::vector<Entity3D*> _children;

public:
	Entity3D();
	~Entity3D();

	virtual Entity3D* clone() const = 0;
	virtual Entity3D& operator=(const Entity3D& e);
	virtual void copyAttributes(const Entity3D& e);

	Entity3D* getParent();
	const Entity3D* getParent() const;
	std::vector<Entity3D*>& getChildren();
	const std::vector<Entity3D*>& getChildren() const;

	void addChild(Entity3D* child);

	// Position Absolute
	void setPositionAbsoluteUpdateChildren(float x, float y, float z);
	void setPositionAbsoluteUpdateChildren(glm::vec3 pos);
	// Position Locale
	void setPositionLocaleUpdateChildren(float x, float y, float z);
	void setPositionLocaleUpdateChildren(glm::vec3 pos);

	// Rotation Absolute
	void setRotationAbsoluteUpdateChildren(float rx, float ry, float rz);
	void setRotationAbsoluteUpdateChildren(glm::vec3 rot);
	
	// Rotation Locale
	void setRotationLocaleUpdateChildren(float rx, float ry, float rz);
	void setRotationLocaleUpdateChildren(glm::vec3 rot);

	// Scale Absolute
	void setScaleAbsoluteUpdateChildren(float sx, float sy, float sz);
	void setScaleAbsoluteUpdateChildren(glm::vec3 scale);

	// Scale Locale
	void setScaleLocaleUpdateChildren(float sx, float sy, float sz);
	void setScaleLocaleUpdateChildren(glm::vec3 scale);	

	glm::vec3 getPositionAbsolute() const;
	glm::vec3 getPositionLocale() const;

	glm::vec3 getRotationAbsolute() const;
	glm::vec3 getRotationLocale() const;

	glm::vec3 getScaleAbsolute() const;
	glm::vec3 getScaleLocale() const;

	glm::mat4 getTranslate() const;
	glm::mat4 getTranslateLocale() const;
	glm::mat4 getRotate() const;
	glm::mat4 getRotateLocale() const;
	glm::mat4 getScale() const;

	glm::mat4 getTransform() const;
	glm::mat4 getTransformParent() const;
	glm::mat4 getTransformLocale() const;
	/**
	* Must call buildTransformWorld() from the father node before 
	*/
	glm::mat4 getTransformWorld() const;

	void buildTransformWorld();

private:
	void setParent(Entity3D* parent);
	void buildTransformWorld(glm::mat4& father_mat);

public:
	// Position Absolute
	void setPositionAbsolute(float x, float y, float z);
	void setPositionAbsolute(glm::vec3 pos);

	// Position Locale
	void setPositionLocale(float x, float y, float z);
	void setPositionLocale(glm::vec3 pos);

	// Rotation Absolute
	void setRotationAbsolute(float rx, float ry, float rz);
	void setRotationAbsolute(glm::vec3 rot);

	// Rotation Locale
	void setRotationLocale(float rx, float ry, float rz);
	void setRotationLocale(glm::vec3 rot);


	// Scale Absolute
	void setScaleAbsolute(float sx, float sy, float sz);
	void setScaleAbsolute(glm::vec3 scale);

	// Scale Locale
	void setScaleLocale(float sx, float sy, float sz);
	void setScaleLocale(glm::vec3 scale);
};

