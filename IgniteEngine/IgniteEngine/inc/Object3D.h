#pragma once

#include "Entity3D.h"
#include "Mesh.h"
#include "DefaultConf.h"
#include "Sampler.h"
#include "Texture2D.h"
#include "RenderFamily.h"
#include "LoadedObjectInfo.h"

#include <map>
#include <unordered_map>
#include <stack>
#include <vector>
#include <sstream>
#include <fstream>

class Object3D;

struct Object3DArrays {
	std::unordered_map<Mesh*, std::vector<Object3D*>> mesh_objects;
	std::vector<glm::vec3> coords;
	std::vector<uint32_t> mesh_offsets;
	std::vector<uint32_t> object_id;
	std::vector<uint32_t> indices;
	std::vector<Texture2D*> _textures2D;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<uint32_t> transform_indices;
	std::vector<glm::mat4> transform_matrices;
	std::vector<uint32_t> material_indices;
	std::vector<glsl::Mat> materials;
	std::vector<uint32_t> texture2D_indices;
	std::vector<glm::uvec4> joints_ids;
	std::vector<glm::vec4> weights;
	std::vector<glm::mat4> joints_transform;
};

class Object3D : public Entity3D
{
public:
	static const uint32_t DEFAULT_MATERIAL_INDICES = 0;

private:
	static std::unordered_map<RenderFamily*, std::unordered_map<GraphicsPipeline*, Object3DArrays>> _arrays;

	static std::vector<Object3D*> allocated_objects;

	Mesh* _mesh;
	Skeleton* _skeleton;
	RenderFamily* _rf;
	std::vector<uint32_t>* _material_indices;
	std::vector<Material*> _materials;
	std::vector<Texture2D*> _Texture2D;
	std::vector<GraphicsPipeline*> _gps;

public:
	Object3D();

	Entity3D* clone() const;
	Object3D& operator=(const Object3D& o);
	void copyAttributes(const Object3D& o);

	void setMesh(Mesh& mesh);
	const Mesh& getMesh() const;

	void setSkeleton(Skeleton& skeleton);
	Skeleton* getSkeleton();
	const Skeleton* getSkeleton() const;
	
	void setRenderFamily(RenderFamily& rf);
	const RenderFamily& getRenderFamily() const;

	void setMaterial(Material& material);
	const std::vector<Material*>& getMaterial() const;
	std::vector<uint32_t>* getMaterialIndices();

	void setTexture2D(std::vector<Texture2D*>& Texture2D);
	const std::vector<Texture2D*>& getTexture2D() const;

	void addGraphicsPipeline(GraphicsPipeline& gp);
	const GraphicsPipeline& getGraphicsPipeline(uint32_t i) const;
	const std::vector<GraphicsPipeline*>& getGraphicsPipeline() const;

	void createFromObjectInfo(const LoadedObjectInfo& loi);

	void setMaterial(
		const std::vector<Material>& material,
		std::vector<uint32_t>* indices
	);
private:
	void createFromObjectInfo(const LoadedObjectInfo& loi, Object3D* obj);
	void setTextures2D(const std::vector<Texture2D>& Texture2D);

public:
	static const std::unordered_map<RenderFamily*, std::unordered_map<GraphicsPipeline*, Object3DArrays>>& getArrays();
	static const std::unordered_map<GraphicsPipeline*, Object3DArrays>& getArrays(RenderFamily& rf);

	static const std::vector<glm::vec3>& getCoords(RenderFamily& rf, GraphicsPipeline& gp);
	static std::vector<glm::vec3>& updateCoords(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getCoordsStride(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getCoordsSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glm::vec3>& getNormals(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getNormalsStride(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getNormalsSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getObjectId(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getObjectIdStride(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getObjectIdSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getMeshOffsets(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getMeshOffsetsStride(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getMeshOffsetsSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getIndices(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getIndicesSize(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getIndicesNbElem(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glm::vec2>& getUV(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getUVSize(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getUVStride(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glm::uvec4>& getJoints(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getJointsSize(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getJointsStride(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glm::vec4>& getWeights(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getWeightsSize(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getWeightsStride(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<Texture2D*>& getTextures2D(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getTextures2DSize(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getTextures2DStride(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getTransformIndices(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getTransformIndicesSize(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getTransformIndicesStride(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glm::mat4>& getTransformMatrices(RenderFamily& rf, GraphicsPipeline& gp);
	static std::vector<glm::mat4>& updateTransformMatrices(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getTransformMatricesSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getMaterialIndices(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getMaterialIndicesStride(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getMaterialIndicesSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glsl::Mat>& getMaterials(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getMaterialsStride(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getMaterialsSize(RenderFamily& rf, GraphicsPipeline& gp);

	static std::vector<glm::mat4>& getJointsTransform(RenderFamily& rf, GraphicsPipeline& gp);
	static std::vector<glm::mat4>& updateJointsTransform(RenderFamily& rf, GraphicsPipeline& gp);
	static uint32_t getJointsTransformSize(RenderFamily& rf, GraphicsPipeline& gp);

	//static std::vector<uint32_t>& getTexture2DIndices(RenderFamily& rf, GraphicsPipeline& gp);
	//static uint32_t getTexture2DIndicesStride(RenderFamily& rf, GraphicsPipeline& gp);
	//static uint32_t getTexture2DIndicesSize(RenderFamily& rf, GraphicsPipeline& gp);

private:
	static void buildCoords(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildNormals(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildObjectId(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildMeshOffsets(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildIndices(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildUV(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildJoints(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildWeights(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildTextures2D(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildTransformIndices(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildTransformMatrices(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildMaterialIndices(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildMaterials(RenderFamily& rf, GraphicsPipeline& gp);
	static void buildJointsTransform(RenderFamily& rf, GraphicsPipeline& gp);
	//static void buildTexture2DIndices(RenderFamily& rf, GraphicsPipeline& gp);

	static void freeAllocatedObjects();
};