#pragma once
#include "Module.h"
#include "Mesh.h"
#include "Object3D.h"
#include "LoadedObjectInfo.h"
#include "DispatcherSync.h"

class TestModule : public Module
{
private:
    Object3D _obj;
    Object3D _obj2;
    Object3D _obj3;
    Object3D _flying_battle;
    Object3D _cube_obj;
    Object3D _cornell_obj;

    LoadedObjectInfo _cube_info;
    LoadedObjectInfo _cornell_info;
    LoadedObjectInfo _flying_battle_info;

    Mesh _m;
    uint32_t _frame;

    Texture _tex;
    Texture _mercedes_tex;

    GraphicShader _red_shader;

    VertexBuffer _object_id_buffer;
    VertexBuffer _coord_buffer;
    VertexBuffer _uv_buffer;
    IndexBuffer _index_buffer;
    UniformBuffer _camera_buffer;
    StorageBuffer _obj_tr_buffer;
    StorageBuffer _texture_i_buffer;
    Sampler _sampler;

    std::vector<int32_t> read_arr;
    std::vector<int32_t> write_arr;
    ComputeShader _sum_shader;
    ComputePipeline _sum_pipeline;
    DispatcherSync _dispatcher;

    StorageBuffer _read_buffer;
    StorageBuffer _write_buffer;

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

