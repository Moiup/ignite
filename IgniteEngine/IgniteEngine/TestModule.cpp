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
    _tex.readFile("../assets/textures/scarecrow.png");
    _tex.setLogicalDevice(DefaultConf::logical_device);
    _tex.setGPU(DefaultConf::gpu);
    _tex.setCommandPool(DefaultConf::command_pool);
    _tex.create();

    // -- Meshes -- //
    // Loading the mesh (file)
    _cube_info.loadObj("../assets/3d_objects/cube.obj");
    //_cube_info.loadObj("../3d_objects/output_rand_0_bone.obj");

    // Loading the mesh (by hand)
    _m.setVertex(rectangle(), rectIndices(), rectUV());

    // -- Objects -- //
    // Setting parent Object children
    _obj.addChild(&_obj2);
    _obj.addChild(&_obj3);

    // Configuring first object
    _obj.setMesh(&_m);
    _obj.setRenderer(DefaultConf::renderer);
    _obj.addShader(&_red_shader);
    _obj.setTexture(&_tex);
    _obj.setScaleAbsolute(0.5, 0.5, 0.5);
    _obj.setPositionAbsolute(-2.3f, 0, 0);

    // Configuring second object
    _obj2.setMesh(&_m);
    _obj2.setRenderer(DefaultConf::renderer);
    _obj2.addShader(DefaultConf::graphic_shader);
    _obj2.setPositionAbsolute(1.0, 0.0, 1.0);

     //Configuring third object
    _obj3.setMesh(&_m);
    _obj3.setRenderer(DefaultConf::renderer);
    _obj3.addShader(&_red_shader);
    _obj3.setTexture(&_tex);
    _obj3.setPositionAbsolute(2.3f, 0.0, 0.0);

    _cube_obj.createFromObjectInfo(_cube_info);
    _cube_obj.setRenderer(DefaultConf::renderer);
    _cube_obj.addShader(DefaultConf::graphic_shader);
    _cube_obj.setScaleAbsolute(0.5, 0.5, 0.5);
    _cube_obj.setPositionAbsolute(-2.3f, 0, 0);
    
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
            Object3D::getCoordsStride(DefaultConf::renderer, &_red_shader),
            VK_FORMAT_R32G32B32_SFLOAT,
            0
        );
        _red_shader.addVertexBufferInfo(
            "object_id",
            Object3D::getObjectIdStride(DefaultConf::renderer, &_red_shader),
            VK_FORMAT_R32_UINT,
            1
        );
        _red_shader.addVertexBufferInfo(
            "uv",
            Object3D::getUVStride(DefaultConf::renderer, &_red_shader),
            VK_FORMAT_R32G32_SFLOAT,
            2
        );

        // Index Buffer
        _red_shader.addIndexBufferInfo(
            "index",
            Object3D::getIndicesNbElem(DefaultConf::renderer, &_red_shader)
        );

        // Uniform buffer
        _red_shader.addUniformBufferInfo(
            "camera",
            0,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        // Storage Buffers
        _red_shader.addStorageBufferInfo(
            "obj_tr",
            1,
            VK_SHADER_STAGE_VERTEX_BIT
        );

        _red_shader.addStorageBufferInfo(
            "texture_i",
            2,
            VK_SHADER_STAGE_VERTEX_BIT
        );


        // -- Fragment shader -- //
        _red_shader.addSamplerInfo(
            "samp",
            3,
            VK_SHADER_STAGE_FRAGMENT_BIT
        );
        
        _red_shader.addTextureInfo(
            "textures",
            4,
            VK_SHADER_STAGE_FRAGMENT_BIT
        );

        // Creating the buffers
        // Coord buffer
        _coord_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _coord_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _coord_buffer.setSize(Object3D::getCoordsSize(DefaultConf::renderer, &_red_shader));
        _coord_buffer.create();
        _coord_buffer.setValues(Object3D::getCoords(DefaultConf::renderer, &_red_shader).data());
        _red_shader.addVertexBuffer("coord", &_coord_buffer);
        
        // Mesh offsets
        _object_id_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _object_id_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _object_id_buffer.setSize(Object3D::getObjectIdSize(DefaultConf::renderer, &_red_shader));
        _object_id_buffer.create();
        _object_id_buffer.setValues(Object3D::getObjectId(DefaultConf::renderer, &_red_shader).data());
        _red_shader.addVertexBuffer("object_id", &_object_id_buffer);

        // UV buffer
        _uv_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _uv_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _uv_buffer.setSize(Object3D::getUVSize(DefaultConf::renderer, &_red_shader));
        _uv_buffer.create();
        _uv_buffer.setValues(Object3D::getUV(DefaultConf::renderer, &_red_shader).data());
        _red_shader.addVertexBuffer("uv", &_uv_buffer);

        // Index buffer
        _index_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _index_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _index_buffer.setSize(Object3D::getIndicesSize(DefaultConf::renderer, &_red_shader));
        _index_buffer.create();
        _index_buffer.setValues(Object3D::getIndices(DefaultConf::renderer, &_red_shader).data());
        _red_shader.addIndexBuffer("index", &_index_buffer);

        // Uniform buffer
        _camera_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _camera_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _camera_buffer.setSize(sizeof(DefaultConf::camera->getPerspectiveCamera().getMVP()));
        _camera_buffer.create();
        _camera_buffer.setValues(&DefaultConf::camera->getPerspectiveCamera().getMVP()[0][0]);
        _red_shader.addUniformBuffer("camera", &_camera_buffer);

        // Storage Buffers
        _obj_tr_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _obj_tr_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _obj_tr_buffer.setSize(Object3D::getTransformMatricesSize(DefaultConf::renderer, &_red_shader));
        _obj_tr_buffer.create();
        _obj_tr_buffer.setValues(Object3D::getTransformMatrices(DefaultConf::renderer, &_red_shader).data());
        _red_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);

        _texture_i_buffer.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _texture_i_buffer.setMemoryProperties(DefaultConf::gpu->getMemoryProperties());
        _texture_i_buffer.setSize(Object3D::getTextureIndicesSize(DefaultConf::renderer, &_red_shader));
        _texture_i_buffer.create();
        _texture_i_buffer.setValues(Object3D::getTextureIndices(DefaultConf::renderer, &_red_shader).data());
        _red_shader.addStorageBuffer("texture_i", &_texture_i_buffer);

        // Sampler
        _sampler.setLogicalDevice((VkDevice*)DefaultConf::logical_device->getDevice());
        _sampler.create();
        _red_shader.addSampler("samp", &_sampler);

        // Textures
        _red_shader.addTexture("textures", Object3D::getTextures(DefaultConf::renderer, &_red_shader));
    }

    _frame = 0;
}

void TestModule::update() {
    //_obj.setPositionAbsolute(-2, 0, 0);
    
    //_obj.setPositionAbsoluteUpdateChildren(-2.3f, ((float)_frame - 100) / 100, 0);
    //_obj.setPositionAbsolute(-2.3f, ((float)_frame - 100) / 100, 0);
    //_obj2.setPositionAbsolute(0.0f, ((float)_frame - 100) / 100, 0);
    //_obj3.setPositionAbsolute(2.3f, ((float)_frame - 100) / 100, 0);


    _obj_tr_buffer.setValues(
        &Object3D::updateTransformMatrices(DefaultConf::renderer, &_red_shader)[0][0]
    );

    _camera_buffer.setValues(&DefaultConf::camera->getPerspectiveCamera().getMVP()[0][0]);

    _frame = (_frame + 1) % 200;
}

void TestModule::close() {
    Module::close();

    //_object_id_buffer.destroy();
    //_coord_buffer.destroy();
    //_uv_buffer.destroy();
    //_index_buffer.destroy();
    //_camera_buffer.destroy();
    //_obj_tr_buffer.destroy();
    //_texture_i_buffer.destroy();
    //_sampler.destroy();
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