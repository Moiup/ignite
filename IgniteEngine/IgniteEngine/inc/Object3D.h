#pragma once

#include "Entity3D.h"
#include "Mesh.h"
#include "DefaultConf.h"
#include "Sampler.h"
#include "Texture2D.h"
#include "Renderer.h"
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
	static std::unordered_map<Renderer*, std::unordered_map<GraphicsPipeline*, Object3DArrays>> _arrays;

	static std::vector<Object3D*> allocated_objects;

	Mesh* _mesh;
	Skeleton* _skeleton;
	Renderer* _renderer;
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
	const Skeleton* getSkeleton() const;
	
	void setRenderer(Renderer& renderer);
	const Renderer* getRenderer() const;

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
	static const std::unordered_map<Renderer*, std::unordered_map<GraphicsPipeline*, Object3DArrays>>& getArrays();
	static const std::unordered_map<GraphicsPipeline*, Object3DArrays>& getArrays(Renderer& renderer);

	static const std::vector<glm::vec3>& getCoords(Renderer& renderer, GraphicsPipeline& graphics_pipeline);
	static std::vector<glm::vec3>& updateCoords(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getCoordsStride(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getCoordsSize(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getObjectId(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getObjectIdStride(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getObjectIdSize(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getMeshOffsets(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getMeshOffsetsStride(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getMeshOffsetsSize(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getIndices(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getIndicesSize(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getIndicesNbElem(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<glm::vec2>& getUV(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getUVSize(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getUVStride(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<glm::uvec4>& getJoints(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getJointsSize(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getJointsStride(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<glm::vec4>& getWeights(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getWeightsSize(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getWeightsStride(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<Texture2D*>& getTextures2D(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getTextures2DSize(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getTextures2DStride(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getTransformIndices(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getTransformIndicesSize(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getTransformIndicesStride(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<glm::mat4>& getTransformMatrices(Renderer& renderer, GraphicsPipeline& gp);
	static std::vector<glm::mat4>& updateTransformMatrices(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getTransformMatricesSize(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<uint32_t>& getMaterialIndices(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getMaterialIndicesStride(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getMaterialIndicesSize(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<glsl::Mat>& getMaterials(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getMaterialsStride(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getMaterialsSize(Renderer& renderer, GraphicsPipeline& gp);

	static std::vector<glm::mat4>& getJointsTransform(Renderer& renderer, GraphicsPipeline& gp);
	static std::vector<glm::mat4>& updateJointsTransform(Renderer& renderer, GraphicsPipeline& gp);
	static uint32_t getJointsTransformSize(Renderer& renderer, GraphicsPipeline& gp);

	//static std::vector<uint32_t>& getTexture2DIndices(Renderer& renderer, GraphicsPipeline& gp);
	//static uint32_t getTexture2DIndicesStride(Renderer& renderer, GraphicsPipeline& gp);
	//static uint32_t getTexture2DIndicesSize(Renderer& renderer, GraphicsPipeline& gp);

private:
	static void buildCoords(Renderer& renderer, GraphicsPipeline& gp);
	static void buildObjectId(Renderer& renderer, GraphicsPipeline& gp);
	static void buildMeshOffsets(Renderer& renderer, GraphicsPipeline& gp);
	static void buildIndices(Renderer& renderer, GraphicsPipeline& gp);
	static void buildUV(Renderer& renderer, GraphicsPipeline& gp);
	static void buildJoints(Renderer& renderer, GraphicsPipeline& gp);
	static void buildWeights(Renderer& renderer, GraphicsPipeline& gp);
	static void buildTextures2D(Renderer& renderer, GraphicsPipeline& gp);
	static void buildTransformIndices(Renderer& renderer, GraphicsPipeline& gp);
	static void buildTransformMatrices(Renderer& renderer, GraphicsPipeline& gp);
	static void buildMaterialIndices(Renderer& renderer, GraphicsPipeline& gp);
	static void buildMaterials(Renderer& renderer, GraphicsPipeline& gp);
	static void buildJointsTransform(Renderer& renderer, GraphicsPipeline& gp);
	//static void buildTexture2DIndices(Renderer& renderer, GraphicsPipeline& gp);

	static void freeAllocatedObjects();
};