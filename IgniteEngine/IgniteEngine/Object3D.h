#pragma once

#include "Entity3D.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Texture.h"
#include "Renderer.h"
#include "LoadedObjectInfo.h"

#include <map>
#include <unordered_map>
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

	static std::vector<Object3D*> allocated_objects;

	Mesh* _mesh;
	Renderer* _renderer;
	Texture* _texture;
	std::vector<GraphicShader*> _shaders;

public:
	Object3D();

	void setMesh(Mesh* mesh);
	const Mesh* getMesh() const;

	void setRenderer(Renderer* renderer);
	const Renderer* getRenderer() const;

	void setTexture(Texture* texture);
	const Texture* getTexture() const;

	void addShader(GraphicShader* shader);
	GraphicShader* getShader(uint32_t i);
	std::vector<GraphicShader*>& getShaders();

	void createFromObjectInfo(const LoadedObjectInfo& loi);

private:
	void createFromObjectInfo(const LoadedObjectInfo& loi, Object3D* obj);

public:
	static std::unordered_map<Mesh*, std::vector<Object3D*>>& getMeshObjects(Renderer* renderer, GraphicShader* shader);
	static std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>& getMeshObjects(Renderer* renderer);
	
	static std::vector<glm::vec3>& getCoords(Renderer* renderer, GraphicShader* shader);
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

	static std::vector<uint32_t>& getTextureIndices(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTextureIndicesStride(Renderer* renderer, GraphicShader* shader);
	static uint32_t getTextureIndicesSize(Renderer* renderer, GraphicShader* shader);

private:
	static void buildCoords(Renderer* renderer, GraphicShader* shader);
	static void buildObjectId(Renderer* renderer, GraphicShader* shader);
	static void buildMeshOffsets(Renderer* renderer, GraphicShader* shader);
	static void buildIndices(Renderer* renderer, GraphicShader* shader);
	static void buildUV(Renderer* renderer, GraphicShader* shader);
	static void buildTextures(Renderer* renderer, GraphicShader* shader);
	static void buildTransformIndices(Renderer* renderer, GraphicShader* shader);
	static void buildTransformMatrices(Renderer* renderer, GraphicShader* shader);
	static void buildMaterialIndices(Renderer* renderer, GraphicShader* shader);
	static void buildMaterials(Renderer* renderer, GraphicShader* shader);
	static void buildTextureIndices(Renderer* renderer, GraphicShader* shader);

	static void freeAllocatedObjects();
};
