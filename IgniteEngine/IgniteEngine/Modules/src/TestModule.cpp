//#include "TestModule.h"
//
//
//TestModule::TestModule() :
//    Module::Module()
//{
//    ;
//}
//
//void TestModule::init() {
//    Module::init();
//}
//
//void TestModule::start() {
//    Module::start();
//
//    //_obj_info.loadWavefont("../../assets/3d_objects/cube_textured/cube_tex.obj");
//    _obj_info.loadGLTF("../../assets/3d_objects/gltf/Hand/hand.gltf");
//
//    _obj1.createFromObjectInfo(_obj_info);
//    _obj1.setRenderer(DefaultConf::renderer);
//    _obj1.addShader(DefaultConf::graphic_shader);
//
//    _obj1.setScaleLocale(0.05, 0.05, 0.05);
//
//    //_obj2.createFromObjectInfo(_obj_info);
//    //_obj2.setRenderer(DefaultConf::renderer);
//    //_obj2.addShader(DefaultConf::graphic_shader);
//
//    //_obj3.createFromObjectInfo(_obj_info);
//    //_obj3.setRenderer(DefaultConf::renderer);
//    //_obj3.addShader(DefaultConf::graphic_shader);
//
//    //_obj4.createFromObjectInfo(_obj_info);
//    //_obj4.setRenderer(DefaultConf::renderer);
//    //_obj4.addShader(DefaultConf::graphic_shader);
//
//    ////_obj2.setPositionAbsoluteUpdateChildren(0, 2.5, 0);
//    //_obj1.addChild(&_obj2);
//    //_obj2.addChild(&_obj3);
//    //_obj3.addChild(&_obj4);
//
//
//    //displayGlmVec(_obj4.getPositionAbsolute());
//    //_obj2.setPositionLocaleUpdateChildren(0, 2.5, 0);
//    //displayGlmVec(_obj4.getPositionAbsolute());
//    //_obj3.setPositionLocaleUpdateChildren(0, 2.5, 0);
//    //displayGlmVec(_obj4.getPositionAbsolute());
//    //_obj4.setPositionLocaleUpdateChildren(0, 2.5, 0);
//    //
//    //_obj2.setRotationLocaleUpdateChildren(1, 0, 0);
//    //_obj3.setRotationLocaleUpdateChildren(1, 0, 0);
//    //std::cout << "obj 2: ";
//    //displayGlmVec(_obj2.getRotationAbsolute());
//    //std::cout << "obj 3: ";
//    //displayGlmVec(_obj3.getRotationAbsolute());
//    //std::cout << "obj 4: ";
//    //displayGlmVec(_obj4.getRotationAbsolute());
//    //_obj3.setRotationLocaleUpdateChildren(1, 0, 0);
//}
//
//void TestModule::update() {
//    
//
//}
//
//void TestModule::close() {
//    Module::close();
//
//
//}
//
//void TestModule::displayGlmVec(glm::vec3 v) {
//    std::cout << v.x << " " << v.y << " " << v.z << std::endl;
//}
//
//std::vector<glm::vec3> TestModule::rectangle() {
//    std::vector<glm::vec3> rect{
//        glm::vec3(-1, -1, 0),
//        glm::vec3(1, -1, 0),
//        glm::vec3(1, 1, 0),
//        glm::vec3(-1, 1, 0)
//    };
//    return rect;
//}
//
//std::vector<glm::vec2> TestModule::rectUV() {
//    std::vector<glm::vec2> uv{
//        glm::vec2(0, 1),
//        glm::vec2(1, 1),
//        glm::vec2(1, 0),
//        glm::vec2(0, 0)
//    };
//    return uv;
//}
//
//std::vector<uint32_t> TestModule::rectIndices() {
//    std::vector<uint32_t> ind{
//        0, 1, 3,
//        1, 2, 3
//    };
//    return ind;
//}