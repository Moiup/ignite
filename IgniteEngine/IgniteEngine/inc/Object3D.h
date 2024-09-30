#pragma once

#include "Entity3D.h"
#include "Mesh.h"
#include "DefaultConf.h"
#include "Sampler.h"
#include "Texture.h"
#include "Renderer.h"
#include "LoadedObjectInfo.h"

#include <map>
#include <unordered_map>
#include <stack>
#include <vector>
#include <sstream>
#include <fstream>

class Object3D : public Entity3D
{
public:
	static const uint32_t DEFAULT_MATERIAL_INDICES = 0;

private:
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>> mesh_objects;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec3>>> coords;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> mesh_offsets;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> object_id;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> indices;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<Texture*>>> _textures;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec2>>> uv;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> transform_indices;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::mat4>>> transform_matrices;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> material_indices;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glsl::Mat>>> materials;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> texture_indices;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::uvec4>>> joints_ids;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec4>>> weights;
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::mat4>>> joints_transform;

	static std::vector<Object3D*> allocated_objects;

	Mesh* _mesh;
	Skeleton* _skeleton;
	Renderer* _renderer;
	std::vector<uint32_t>* _material_indices;
	std::vector<Material*> _materials;
	std::vector<Texture*> _texture;
	std::vector<GraphicShader*> _shaders;

public:
	Object3D();

	void setMesh(Mesh* mesh);
	const Mesh* getMesh() const;

	void setSkeleton(Skeleton& skeleton);
	const Skeleton* getSkeleton() const;
	
	void setRenderer(Renderer* renderer);
	const Renderer* getRenderer() const;

	void setMaterial(Material* material);
	const std::vector<Material*>& getMaterial() const;
	std::vector<uint32_t>* getMaterialIndices();

	void setTexture(std::vector<Texture*>& texture);
	const std::vector<Texture*>& getTexture() const;

	void addShader(GraphicShader* shader);
	GraphicShader* getShader(uint32_t i);
	std::vector<GraphicShader*>& getShaders();

	void createFromObjectInfo(const LoadedObjectInfo& loi);

private:
	void createFromObjectInfo(const LoadedObjectInfo& loi, Object3D* obj);
	void setMaterial(
		const std::vector<Material>& material,
		std::vector<uint32_t>* indices
	);
	void setTextures(const std::vector<Texture>& texture);

public:
	static std::unordered_map<Mesh*, std::vector<Object3D*>>& getMeshObjects(Renderer* renderer, GraphicShader* shader);
	static std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>& getMeshObjects(Renderer* renderer);
	static std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>>& getMeshObjects();

	static std::vector<glm::vec3>& getCoords(Renderer* renderer, GraphicShader* shader);
	static std::vector<glm::vec3>& updateCoords(Renderer* renderer, GraphicShader* shader);
	static uint32_t getCoordsStride(Renderer* renderer, GraphicShader* shader);
	static uint32_t getCoordsSize(Renderer* renderer, GraphicShader* shader);

	static std::vector<uint32_t>& getObjectId(Renderer* renderer, GraphicShader* shader);
	static uint32_t getObjectIdStride(Renderer* renderer, GraphicShader* shader);
	static uint32_t getObjectIdSize(Renderer* renderer, GraphicShader* shader);

	static std::vector<uint32_t>& getMeshOffsets(Renderer* renderer, GraphicShader* shader);
	static uint32_t getMeshOffsetsStride(Renderer* renderer, GraphicShader* shader);
	static uint32_t getMeshOffsetsSize(Renderer* renderer, GraphicShader* shader);

	static std::vector<uint32_t>& getIndices(Renderer* renderer, GraphicShader* shader);
	static uint32_t getIndicesSize(Renderer* renderer, GraphicShader* shader);
	static uint32_t getIndicesNbElem(Renderer* renderer, GraphicShader* shader);

	static std::vector<glm::vec2>& getUV(Renderer* renderer, GraphicShader* shader);
	static uint32_t getUVSize(Renderer* renderer, GraphicShader* shader);
	static uint32_t getUVStride(Renderer* renderer, GraphicShader* shader);

	static std::vector<glm::uvec4>& getJoints(Renderer* renderer, GraphicShader* shader);
	static uint32_t getJointsSize(Renderer* renderer, GraphicShader* shader);
	static uint32_t getJointsStride(Renderer* renderer, GraphicShader* shader);

	static std::vector<glm::vec4>& getWeights(Renderer* renderer, GraphicShader* shader);
	static uint32_t getWeightsSize(Renderer* renderer, GraphicShader* shader);
	static uint32_t getWeightsStride(Renderer* renderer, GraphicShader* shader);

	static std::vector<Texture*>& getTextures(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTexturesSize(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTexturesStride(Renderer* renderer, GraphicShader* shader);

	static std::vector<uint32_t>& getTransformIndices(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTransformIndicesSize(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTransformIndicesStride(Renderer* renderer, GraphicShader* shader);

	static std::vector<glm::mat4>& getTransformMatrices(Renderer* renderer, GraphicShader* shader);
	static std::vector<glm::mat4>& updateTransformMatrices(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTransformMatricesSize(Renderer* renderer, GraphicShader* shader);

	static std::vector<uint32_t>& getMaterialIndices(Renderer* renderer, GraphicShader* shader);
	static uint32_t getMaterialIndicesStride(Renderer* renderer, GraphicShader* shader);
	static uint32_t getMaterialIndicesSize(Renderer* renderer, GraphicShader* shader);

	static std::vector<glsl::Mat>& getMaterials(Renderer* renderer, GraphicShader* shader);
	static uint32_t getMaterialsStride(Renderer* renderer, GraphicShader* shader);
	static uint32_t getMaterialsSize(Renderer* renderer, GraphicShader* shader);

	static std::vector<glm::mat4>& getJointsTransform(Renderer* renderer, GraphicShader* shader);
	static std::vector<glm::mat4>& updateJointsTransform(Renderer* renderer, GraphicShader* shader);
	static uint32_t getJointsTransformSize(Renderer* renderer, GraphicShader* shader);

	//static std::vector<uint32_t>& getTextureIndices(Renderer* renderer, GraphicShader* shader);
	//static uint32_t getTextureIndicesStride(Renderer* renderer, GraphicShader* shader);
	//static uint32_t getTextureIndicesSize(Renderer* renderer, GraphicShader* shader);

private:
	static void buildCoords(Renderer* renderer, GraphicShader* shader);
	static void buildObjectId(Renderer* renderer, GraphicShader* shader);
	static void buildMeshOffsets(Renderer* renderer, GraphicShader* shader);
	static void buildIndices(Renderer* renderer, GraphicShader* shader);
	static void buildUV(Renderer* renderer, GraphicShader* shader);
	static void buildJoints(Renderer* renderer, GraphicShader* shader);
	static void buildWeights(Renderer* renderer, GraphicShader* shader);
	static void buildTextures(Renderer* renderer, GraphicShader* shader);
	static void buildTransformIndices(Renderer* renderer, GraphicShader* shader);
	static void buildTransformMatrices(Renderer* renderer, GraphicShader* shader);
	static void buildMaterialIndices(Renderer* renderer, GraphicShader* shader);
	static void buildMaterials(Renderer* renderer, GraphicShader* shader);
	static void buildJointsTransform(Renderer* renderer, GraphicShader* shader);
	//static void buildTextureIndices(Renderer* renderer, GraphicShader* shader);

	static void freeAllocatedObjects();
};