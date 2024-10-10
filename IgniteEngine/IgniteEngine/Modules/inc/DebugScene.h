#pragma once
#include "Module.h"
#include "DefaultDispatcher.h"

class DebugScene : public Module
{
private:

	VertexBuffer _coord_buffer{};
	VertexBuffer _object_id_buffer{};
	VertexBuffer _material_indices_buffer{};
	VertexBuffer _uv_buffer{};
	IndexBuffer _index_buffer{};
	//UniformBuffer _camera_buffer;
	StorageBuffer _obj_tr_buffer{};
	StorageBuffer _materials_buffer{};
	Sampler _sampler{};

	glm::mat4 _camera;

	Mesh _cross_mesh;
	std::vector<Material> _cross_material;
	std::vector<uint32_t> _cross_material_indices;
	Object3D _cross_obj;

public:
	GraphicShader _debug_shader;

public:
    DebugScene();

    void init();
    void start();
    void update();
    void close();

	static void createCrossMesh(Mesh& cross_mesh, float size);
	static void createCrossMaterial(std::vector<Material>& cross_material, std::vector<uint32_t>& cross_material_indices);

private:
	void createShader();
	
};

