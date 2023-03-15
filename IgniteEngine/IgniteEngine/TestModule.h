#pragma once
#include "Module.h"
#include "Mesh.h"
#include "Object3D.h"

class TestModule : public Module
{
private:
    Object3D _obj;
    Object3D _obj2;
    Object3D _obj3;

    Mesh m;
    uint32_t _frame;

public:
    TestModule();

	void init();
	void start();
	void update();
    void close();

    std::vector<glm::vec3> rectangle();

    std::vector<uint32_t> rectIndices();
};

