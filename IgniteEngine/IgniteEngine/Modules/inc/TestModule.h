#pragma once
#include "Module.h"
#include "DefaultDispatcher.h"

class TestModule : public Module
{
private:
    Object3D _obj1;
    Object3D _obj2;
    Object3D _obj3;
    Object3D _obj4;

    LoadedObjectInfo _obj_info;


public:
    TestModule();

	void init();
	void start();
	void update();
    void close();

    std::vector<glm::vec3> rectangle();
    std::vector<glm::vec2> rectUV();
    std::vector<uint32_t> rectIndices();

private:
    void displayGlmVec(glm::vec3 v);
};

