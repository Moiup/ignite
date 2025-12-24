#pragma once

#include "Skeleton.h"
#include "Object3D.h"

class SkeletonDebug {
private:
	Object3D* _object;
	RenderFamily* _rf;
	GraphicsPipeline* _pipeline;
	Mesh _cross_mesh;
	std::vector<Object3D> _cross_objs;
	std::vector<Material> _cross_material;
	std::vector<uint32_t> _cross_material_indices;
	float _size;

public:
	SkeletonDebug() = default;

	void setObject3D(Object3D& object);
	void setRenderFamily(RenderFamily& rf);
	void setPipeline(GraphicsPipeline& pipeline);
	void setSize(float size=1.0f);
	void create();
	void update();

	std::vector<Object3D>& getCrossObjs();

	static void createCrossMesh(Mesh& cross_mesh, float size = 1.0f);
	static void createCrossMaterial(
		std::vector<Material>& cross_material,
		std::vector<uint32_t>& cross_material_indices
	);
};