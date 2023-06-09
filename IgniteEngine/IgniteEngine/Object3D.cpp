#include "Object3D.h"

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>> Object3D::mesh_objects{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec3>>> Object3D::coords{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::mesh_offsets{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::object_id;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::indices{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec2>>> Object3D::uv{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Texture*, std::vector<Object3D*>>>> Object3D::textures_obj;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<Texture*>>> Object3D::_textures;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::transform_indices;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::mat4>>> Object3D::transform_matrices;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::texture_indices;

std::vector<Object3D*> Object3D::allocated_objects;

Object3D::Object3D() :
	_mesh{ nullptr },
	_renderer{ nullptr },
	_shaders{ nullptr },
	_texture{ nullptr }
{ ; }

void Object3D::createFromObjectInfo(const LoadedObjectInfo& loi) {
	createFromObjectInfo(loi, this);
}

void Object3D::createFromObjectInfo(const LoadedObjectInfo& loi, Object3D* obj) {
	obj->setMesh((Mesh*)&loi._mesh);
	
	for (const auto& l : loi._children) {
		Object3D* o = new Object3D();
		Object3D::allocated_objects.push_back(o);
		createFromObjectInfo(l, o);
		obj->addChild(o);
	}
}

void Object3D::setMesh(Mesh* mesh) {
	uint32_t i = 0;
	// For the previous mesh
	for (GraphicShader* shader : _shaders) {
		for (Object3D* obj : Object3D::mesh_objects[_renderer][shader][_mesh]) {
			// Find the object
			if (obj != this) {
				i++;
				continue;
			}
			// Remove it from the vector associated to this mesh
			Object3D::mesh_objects[_renderer][shader][_mesh].erase(
				Object3D::mesh_objects[_renderer][shader][_mesh].begin() + i
			);
		}
		Object3D::mesh_objects[_renderer][shader][mesh].push_back(this);
	}
	_mesh = mesh;
}

const Mesh* Object3D::getMesh() const {
	return _mesh;
}

void Object3D::setRenderer(Renderer* renderer) {
	uint32_t i = 0;
	// For the previous renderer
	// In the corresponding mesh
	for (auto shader : _shaders) {
		for (Object3D* obj : Object3D::mesh_objects[_renderer][shader][_mesh]) {
			// Find the object
			if (obj != this) {
				i++;
				continue;
			}
			// Remove it from the vector associated to this mesh
			Object3D::mesh_objects[_renderer][shader][_mesh].erase(
				Object3D::mesh_objects[_renderer][shader][_mesh].begin() + i
			);
		}
		// Finally, update the renderer and put the object in the right places
		Object3D::mesh_objects[renderer][shader][_mesh].push_back(this);
	}
	_renderer = renderer;
}

void Object3D::setTexture(Texture* texture) {
	uint32_t i = 0;
	for (GraphicShader* shader : _shaders) {
		for (Object3D* obj : Object3D::textures_obj[_renderer][shader][_texture]) {
			if (obj != this) {
				i++;
				continue;
			}
			Object3D::textures_obj[_renderer][shader][_texture].erase(
				Object3D::textures_obj[_renderer][shader][_texture].begin() + i
			);
		}
		Object3D::textures_obj[_renderer][shader][texture].push_back(this);
	}
	_texture = texture;
}

const Texture* Object3D::getTexture() const {
	return _texture;
}

void Object3D::addShader(GraphicShader* shader) {
	if (!_shaders[0]) {
		// remove it
		// if there is already an affected objects (otherwise, segfautl)
		if (Object3D::mesh_objects[_renderer][nullptr][_mesh].size()) {
			Object3D::mesh_objects[_renderer][nullptr][_mesh].erase(
				Object3D::mesh_objects[_renderer][_shaders[0]][_mesh].begin()
			);
		}
	}
	_shaders.push_back(shader);

	Object3D::mesh_objects[_renderer][shader][_mesh].push_back(this);
}

GraphicShader* Object3D::getShader(uint32_t i) {
	return _shaders[i];
}

std::vector<GraphicShader*>& Object3D::getShaders() {
	return _shaders;
}

const Renderer* Object3D::getRenderer() const {
	return _renderer;
}

std::unordered_map<Mesh*, std::vector<Object3D*>>& Object3D::getMeshObjects(Renderer* renderer, GraphicShader* shader) {
	return Object3D::mesh_objects[renderer][shader];
}

std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>& Object3D::getMeshObjects(Renderer* renderer) {
	return Object3D::mesh_objects[renderer];
}

std::vector<glm::vec3>& Object3D::getCoords(Renderer* renderer, GraphicShader* shader) {
	buildCoords(renderer, shader);
	return Object3D::coords[renderer][shader];
}

uint32_t Object3D::getCoordsStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getCoords(renderer, shader).data());
}

uint32_t Object3D::getCoordsSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getCoordsStride(renderer, shader);
	return getCoords(renderer, shader).size() * stride;
}

std::vector<uint32_t>& Object3D::getObjectId(Renderer* renderer, GraphicShader* shader) {
	buildObjectId(renderer, shader);
	return Object3D::object_id[renderer][shader];
}

uint32_t Object3D::getObjectIdStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(getObjectId(renderer, shader).data());
}

uint32_t Object3D::getObjectIdSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getObjectIdStride(renderer, shader);
	return getObjectId(renderer, shader).size() * stride;
}

std::vector<uint32_t>& Object3D::getMeshOffsets(Renderer* renderer, GraphicShader* shader) {
	buildMeshOffsets(renderer, shader);
	return Object3D::mesh_offsets[renderer][shader];
}

uint32_t Object3D::getMeshOffsetsStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getMeshOffsets(renderer, shader).data());
}

uint32_t Object3D::getMeshOffsetsSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getMeshOffsetsStride(renderer, shader);
	return getMeshOffsets(renderer, shader).size() * stride;
}

std::vector<uint32_t>& Object3D::getIndices(Renderer* renderer, GraphicShader* shader) {
	buildIndices(renderer, shader);
	return Object3D::indices[renderer][shader];
}

uint32_t Object3D::getIndicesSize(Renderer* renderer, GraphicShader* shader) {
	return getIndicesNbElem(renderer, shader) * sizeof(*getIndices(renderer, shader).data());
}

uint32_t Object3D::getIndicesNbElem(Renderer* renderer, GraphicShader* shader) {
	return getIndices(renderer, shader).size();
}

std::vector<glm::vec2>& Object3D::getUV(Renderer* renderer, GraphicShader* shader) {
	buildUV(renderer, shader);
	return Object3D::uv[renderer][shader];
}

uint32_t Object3D::getUVStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getUV(renderer, shader).data());
}

uint32_t Object3D::getUVSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getUVStride(renderer, shader);
	return getUV(renderer, shader).size() * stride;
}

std::vector<Texture*>& Object3D::getTextures(Renderer* renderer, GraphicShader* shader) {
	buildTextures(renderer, shader);
	return _textures[renderer][shader];
}

uint32_t Object3D::getTexturesSize(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getTextures(renderer, shader).data());
}

uint32_t Object3D::getTexturesStride(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getTexturesStride(renderer, shader);
	return getTextures(renderer, shader).size() * stride;
}

std::vector<uint32_t>& Object3D::getTransformIndices(Renderer* renderer, GraphicShader* shader) {
	buildTransformIndices(renderer, shader);
	return Object3D::transform_indices[renderer][shader];
}

uint32_t Object3D::getTransformIndicesSize(Renderer* renderer, GraphicShader* shader) {
	return getTransformIndices(renderer, shader).size() * sizeof(*getTransformIndices(renderer, shader).data());
}

uint32_t Object3D::getTransformIndicesStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getTransformIndices(renderer, shader).data());
}

std::vector<glm::mat4>& Object3D::getTransformMatrices(Renderer* renderer, GraphicShader* shader) {
	buildTransformMatrices(renderer, shader);
	return Object3D::transform_matrices[renderer][shader];
}

std::vector<glm::mat4>& Object3D::updateTransformMatrices(Renderer* renderer, GraphicShader* shader) {
	uint32_t mat_i = 0;
	// For each mesh in the renderer
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		 std::vector<Object3D*> objects = m_o.second;
		 uint32_t n = objects.size();
		 
		for (uint32_t i = 0; i < n; i++) {
			Object3D* obj = objects[i];
			Object3D::transform_matrices[renderer][shader][mat_i] = obj->getTransform();
			mat_i++;
		}
	}

	return Object3D::transform_matrices[renderer][shader];
}

uint32_t Object3D::getTransformMatricesSize(Renderer* renderer, GraphicShader* shader) {
	return getTransformMatrices(renderer, shader).size() * sizeof(*getTransformMatrices(renderer, shader).data());
}

std::unordered_map<Texture*, std::vector<Object3D*>>& Object3D::getTextureObjects(Renderer* renderer, GraphicShader* shader) {
	return Object3D::textures_obj[renderer][shader];
}

std::vector<uint32_t>& Object3D::getTextureIndices(Renderer* renderer, GraphicShader* shader) {
	buildTextureIndices(renderer, shader);
	return Object3D::texture_indices[renderer][shader];
}

uint32_t Object3D::getTextureIndicesStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(getTextureIndices(renderer, shader).data());
}

uint32_t Object3D::getTextureIndicesSize(Renderer* renderer, GraphicShader* shader){
	uint32_t stride = getTextureIndicesStride(renderer, shader);
	return getTextureIndices(renderer, shader).size() * stride;
}

void Object3D::buildCoords(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::coords[renderer][shader].size()) {
		return;
	}

	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		const Mesh* mesh = m_o.first;
		Object3D::coords[renderer][shader].insert(
			Object3D::coords[renderer][shader].end(),
			mesh->getCoords().begin(),
			mesh->getCoords().end()
		);
	}
}

void Object3D::buildMeshOffsets(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::mesh_offsets[renderer][shader].size()) {
		return;
	}

	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		Mesh* mesh = m_o.first;
		uint32_t mesh_offset = mesh->getIndicesNbElem();
		for (uint32_t i = 0; i < mesh->getCoords().size(); i++) {
			Object3D::mesh_offsets[renderer][shader].push_back(mesh_offset);
		}
	}
}

void Object3D::buildObjectId(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::object_id[renderer][shader].size()) {
		return;
	}

	uint32_t obj_i{0};
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		Mesh* mesh = m_o.first;
		std::vector<Object3D*>& objects = m_o.second;
		uint32_t nb_coords = mesh->getCoords().size();
		for (Object3D* obj : objects) {
			for (uint32_t i = 0; i < nb_coords; i++) {
				Object3D::object_id[renderer][shader].push_back(obj_i);
			}
		}
		obj_i += objects.size();
	}
}

void Object3D::buildIndices(Renderer* renderer, GraphicShader* shader) {
	uint32_t max = 0;
	
	// if not empty
	if (Object3D::indices[renderer][shader].size()) {
		return;
	}

	// For each mesh
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		const Mesh* mesh = m_o.first;
		uint32_t tmp = 0;
		for (uint32_t i : mesh->getIndices()) {
			if (i > tmp) {
				tmp = i;
			}
			// We adapt and add the indices
			Object3D::indices[renderer][shader].push_back(i + max);
		}
		max = max + tmp + 1;
	}
}

void Object3D::buildUV(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::uv[renderer][shader].size()) {
		return;
	}

	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		const Mesh* mesh = m_o.first;
		Object3D::uv[renderer][shader].insert(
			Object3D::uv[renderer][shader].end(),
			mesh->getUV().begin(),
			mesh->getUV().end()
		);
	}
}


void Object3D::buildTextures(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::_textures[renderer][shader].size()) {
		return;
	}

	// Finding the textures
	for (auto& t_o : textures_obj[renderer][shader]) {
		Texture* tex = t_o.first;
		if (!tex) {
			continue;
		}
		//std::vector<Object3D*>& objs = t_o.second;

		_textures[renderer][shader].push_back(tex);
	}
}

void Object3D::buildTransformIndices(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::transform_indices[renderer][shader].size()) {
		return;
	}

	uint64_t max = 0;
	std::vector<uint32_t>& indices = Object3D::getIndices(renderer, shader);
	for (uint32_t ind : indices) {
		if (ind > max) {
			max = ind;
		}
	}

	uint32_t tr_i = 0;
	max = 0;
	// For each mesh of the renderer
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		if (m_o.first == nullptr) {
			continue;
		}
		// Reach each object
		for (Object3D* obj : m_o.second) {
			const Mesh* mesh = obj->getMesh();
			for (uint32_t i : mesh->getIndices()) {
				Object3D::transform_indices[renderer][shader].push_back(tr_i);
			}
			tr_i++;
		}
	}
}

void Object3D::buildTransformMatrices(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::transform_matrices[renderer][shader].size()) {
		return;
	}

	// For each mesh of the renderer
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		// Reach for each object
		for (Object3D* obj : m_o.second) {
			glm::mat4 tr = obj->getTransform();
			Object3D::transform_matrices[renderer][shader].push_back(tr);
		}
	}
}

void Object3D::buildTextureIndices(Renderer* renderer, GraphicShader* shader) {
	uint32_t tex_i = 0;
	// if not empty
	if (Object3D::texture_indices[renderer][shader].size()) {
		return;
	}

	// For each texture looking for all the objects
	for (auto& t_o : Object3D::textures_obj[renderer][shader]) {
		std::vector<Object3D*> objs = t_o.second;

		// For each object add the index corresponding to the texture
		for (const auto& obj : objs) {
			Object3D::texture_indices[renderer][shader].push_back(tex_i);	
		}
		tex_i++;
	}
}

void Object3D::freeAllocatedObjects() {
	for (Object3D* obj : Object3D::allocated_objects) {
		delete obj;
	}
}