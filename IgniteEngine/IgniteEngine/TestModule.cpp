#include "TestModule.h"


TestModule::TestModule() :
    Module::Module()
{
    ;
}

void TestModule::init() {
    Module::init();
}

void TestModule::start() {
    Module::start();

    // -- Textures -- //
    _tex.readFile("../textures/scarecrow.png");
    _tex.setLogicalDevice(DefaultConf::logical_device);
    _tex.writeFile("../textures/scarecrow_test.png");


    // Loading the mesh
    _m.setCoords(rectangle(), rectIndices(), rectUV());

    // -- Objects -- //
    // Setting parent Object children
    _obj.addChild(&_obj2);
    _obj.addChild(&_obj3);

    // Configuring first object
    _obj.setMesh(&_m);
    _obj.setRenderer(_renderer);
    _obj.addShader(&_red_shader);
    _obj.setScaleAbsolute(0.5, 0.5, 0.5);
    _obj.setPositionAbsolute(-2.3f, 0, 0);

    // Configuring second object
    _obj2.setMesh(&_m);
    _obj2.setRenderer(_renderer);
    _obj2.addShader(_graphics_shader);
    _obj2.setPositionAbsolute(0.0, 0.0, 0.0);

    // Configuring third object
    _obj3.setMesh(&_m);
    _obj3.setRenderer(_renderer);
    _obj3.addShader(&_red_shader);
    _obj3.setPositionAbsolute(2.3f, 0.0, 0.0);


    {
        // Red shader
        _red_shader.setNbFrame(2);
        _red_shader.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _red_shader.setPhysicalDevice(DefaultConf::gpu);
        _red_shader.readShaderFiles(
            "../shaders/vert_red.spv",
            "../shaders/frag_red.spv"
        );
        _red_shader.addVertexBufferInfo(
            "coord",
            Object3D::getCoordsStride(_renderer, &_red_shader),
            VK_FORMAT_R32G32B32_SFLOAT,
            0
        );
        _red_shader.addVertexBufferInfo(
            "mesh_offsets",
            Object3D::getMeshOffsetsStride(_renderer, &_red_shader),
            VK_FORMAT_R32_UINT,
            1
        );
        _red_shader.addVertexBufferInfo(
            "uv",
            Object3D::getUVStride(_renderer, &_red_shader),
            VK_FORMAT_R32G32_SFLOAT,
            2
        );
        // Index Buffer
        _red_shader.addIndexBufferInfo(
            "index",
            Object3D::getIndicesNbElem(_renderer, &_red_shader)
        );

        // Uniform buffer
        _red_shader.addUniformBufferInfo(
            "camera",
            0,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        // Storage Buffers
        _red_shader.addStorageBufferInfo(
            "obj_tr_i",
            1,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        _red_shader.addStorageBufferInfo(
            "obj_tr",
            2,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        // Creating the buffers

        // Mesh offsets
        _mesh_offsets_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _mesh_offsets_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _mesh_offsets_buffer.setSize(Object3D::getMeshOffsetsSize(_renderer, &_red_shader));
        _mesh_offsets_buffer.create();
        _mesh_offsets_buffer.setValues(Object3D::getMeshOffsets(_renderer, &_red_shader).data());
        _red_shader.addVertexBuffer("mesh_offsets", &_mesh_offsets_buffer);

        // Coord buffer
        _coord_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _coord_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _coord_buffer.setSize(Object3D::getCoordsSize(_renderer, &_red_shader));
        _coord_buffer.create();
        _coord_buffer.setValues(Object3D::getCoords(_renderer, &_red_shader).data());
        _red_shader.addVertexBuffer("coord", &_coord_buffer);

        // UV buffer
        _uv_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _uv_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _uv_buffer.setSize(Object3D::getUVSize(_renderer, &_red_shader));
        _uv_buffer.create();
        _uv_buffer.setValues(Object3D::getUV(_renderer, &_red_shader).data());
        _red_shader.addVertexBuffer("uv", &_uv_buffer);

        // Index buffer
        _index_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _index_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _index_buffer.setSize(Object3D::getIndicesSize(_renderer, &_red_shader));
        _index_buffer.create();
        _index_buffer.setValues(Object3D::getIndices(_renderer, &_red_shader).data());
        _red_shader.addIndexBuffer("index", &_index_buffer);

        // Uniform buffer
        _camera_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _camera_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _camera_buffer.setSize(sizeof(_camera->getMVP()));
        _camera_buffer.create();
        _camera_buffer.setValues(&_camera->getMVP()[0][0]);
        _red_shader.addUniformBuffer("camera", &_camera_buffer);

        // Storage Buffers
        _obj_tr_i_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _obj_tr_i_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _obj_tr_i_buffer.setSize(Object3D::getTransformIndicesSize(_renderer, &_red_shader));
        _obj_tr_i_buffer.create();
        _obj_tr_i_buffer.setValues(Object3D::getTransformIndices(_renderer, &_red_shader).data());
        _red_shader.addStorageBuffer("obj_tr_i", &_obj_tr_i_buffer);

        _obj_tr_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _obj_tr_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(_renderer, &_red_shader));
        _obj_tr_buffer.create();
        _obj_tr_buffer.setValues(Object3D::getTransformMatrices(_renderer, &_red_shader).data());
        _red_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);
    }

    _frame = 0;
}

void TestModule::update() {
    //_obj.setPositionAbsolute(-2, 0, 0);
    
    _obj.setPositionAbsoluteUpdateChildren(-2.3f, ((float)_frame - 100) / 100, 0);
    //_obj.setPositionAbsolute(-2.3f, ((float)_frame - 100) / 100, 0);
    //_obj2.setPositionAbsolute(0.0f, ((float)_frame - 100) / 100, 0);
    //_obj3.setPositionAbsolute(2.3f, ((float)_frame - 100) / 100, 0);

    _obj_tr_buffer.setValues(
        Object3D::updateTransformMatrices(_renderer, &_red_shader).data()
    );

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

std::vector<glm::vec2> TestModule::rectUV() {
    std::vector<glm::vec2> uv{
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1),
        glm::vec2(0, 1)
    };
    return uv;
}

std::vector<uint32_t> TestModule::rectIndices() {
    std::vector<uint32_t> ind{
        0, 1, 3,
        1, 2, 3
    };
    return ind;
}