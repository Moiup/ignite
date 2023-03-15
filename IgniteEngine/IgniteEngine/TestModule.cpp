#include "TestModule.h"

TestModule::TestModule() :
    Module::Module()
{
    ;
}

void TestModule::init() {
    Module::init();
    std::cout << "TestModule::init()" << std::endl;
    

}

void TestModule::start() {
    Module::start();
    
    _obj.addChild(&_obj2);
    _obj.addChild(&_obj3);

    m.setCoords(rectangle(), rectIndices());
    _obj.setMesh(&m);
    _obj.setRenderer(_renderer);
    _obj.addShader(_graphics_shader);
    _obj.setScaleAbsolute(0.5, 0.5, 0.5);
    _obj.setPositionAbsolute(-2.3f, 0, 0);

    _obj2.setMesh(&m);
    _obj2.setRenderer(_renderer);
    _obj2.addShader(_graphics_shader);
    _obj2.setPositionAbsolute(0.0, 0.0, 0.0);

    _obj3.setMesh(&m);
    _obj3.setRenderer(_renderer);
    _obj3.addShader(_graphics_shader);
    _obj3.setPositionAbsolute(2.3f, 0.0, 0.0);

    _frame = 0;
}

void TestModule::update() {
    //_obj.setPositionAbsolute(-2, 0, 0);
    
    _obj.setPositionAbsoluteUpdateChildren(-2.3f, ((float)_frame - 100) / 100, 0);
    //_obj.setPositionAbsolute(-2.3f, ((float)_frame - 100) / 100, 0);
    //_obj2.setPositionAbsolute(0.0f, ((float)_frame - 100) / 100, 0);
    //_obj3.setPositionAbsolute(2.3f, ((float)_frame - 100) / 100, 0);

    _frame = (_frame + 1) % 200;
}

void TestModule::close() {
    Module::close();
}

std::vector<glm::vec3> TestModule::rectangle() {
    std::vector<glm::vec3> rect{
        glm::vec3(-1, -1, 0),
        glm::vec3(1, -1, 0),
        glm::vec3(1, 1, 0),
        glm::vec3(-1, 1, 0)
    };
    return rect;
}

std::vector<uint32_t> TestModule::rectIndices() {
    std::vector<uint32_t> ind{
        0, 1, 3,
        1, 2, 3
    };
    return ind;
}