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

    Mesh _m;
    uint32_t _frame;

    Texture _tex;
    GraphicShader _red_shader;

    VertexBuffer _object_id_buffer;
    VertexBuffer _coord_buffer;
    VertexBuffer _uv_buffer;
    IndexBuffer _index_buffer;
    UniformBuffer _camera_buffer;
    StorageBuffer _obj_tr_buffer;
    StorageBuffer _texture_i_buffer;
    Sampler _sampler;

public:
    TestModule();

	void init();
	void start();
	void update();
    void close();

    std::vector<glm::vec3> rectangle();
    std::vector<glm::vec2> rectUV();
    std::vector<uint32_t> rectIndices();
};

