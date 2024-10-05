#include "Object3D.h"

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>> Object3D::mesh_objects{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec3>>> Object3D::coords{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::mesh_offsets{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::object_id;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::indices{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec2>>> Object3D::uv{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<Texture*>>> Object3D::_textures{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::transform_indices{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::mat4>>> Object3D::transform_matrices{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::material_indices{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glsl::Mat>>> Object3D::materials;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<uint32_t>>> Object3D::texture_indices{};

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::uvec4>>> Object3D::joints_ids;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::vec4>>> Object3D::weights;

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::vector<glm::mat4>>> Object3D::joints_transform;

std::vector<Object3D*> Object3D::allocated_objects{};

Object3D::Object3D() :
	_mesh{ nullptr },
	_skeleton{ nullptr },
	_renderer{ nullptr },
	_material_indices{},
	_materials{},
	_shaders{ nullptr },
	_texture{}
{
	;
}

Entity3D* Object3D::clone() const {
	Object3D* o = new Object3D;

	o->copyAttributes(*this);

	for (Entity3D* child : this->_children) {
		Joint* child_o = reinterpret_cast<Joint*>(child);
		o->addChild(child_o->clone());
	}

	return o;
}

Object3D& Object3D::operator=(const Object3D& o) {
	copyAttributes(o);

	for (Entity3D* child : this->_children) {
		Object3D* child_o = reinterpret_cast<Object3D*>(child);
		this->addChild(child_o->clone());
	}

	return *this;
}

void Object3D::copyAttributes(const Object3D& o) {
	Entity3D::copyAttributes(o);

	this->_mesh = o._mesh;
	this->_skeleton = o._skeleton;
	this->_renderer = o._renderer;
	this->_material_indices = o._material_indices;
	this->_materials = o._materials;
	this->_texture = o._texture;
	this->_shaders = o._shaders;
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

void Object3D::setSkeleton(Skeleton& skeleton) {
	_skeleton = &skeleton;
}

const Skeleton* Object3D::getSkeleton() const {
	return _skeleton;
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

void Object3D::setMaterial(Material* material) {
	_materials = { material };
	_material_indices = { 0 };
}

const std::vector<Material*>& Object3D::getMaterial() const {
	return _materials;
}

void Object3D::setTexture(std::vector<Texture*>& texture) {
	_texture = texture;
}

const std::vector<Texture*>& Object3D::getTexture() const {
	return _texture;
}

void Object3D::createFromObjectInfo(const LoadedObjectInfo& loi) {
	createFromObjectInfo(loi, this);
}

void Object3D::createFromObjectInfo(const LoadedObjectInfo& loi, Object3D* obj) {
	if (loi._meshes.size() == 1) {
		obj->setMesh(const_cast<Mesh*>(&loi._meshes[0]));
		if (!loi._materials.empty()) {
			const std::vector<Material>& mat = loi._materials[0];
			obj->setMaterial(
				mat,
				(std::vector<uint32_t>*) & loi._material_indices[0]
			);
		}
		if (!loi._textures.empty()) {
			std::vector<Texture>& tex = (std::vector<Texture>&)loi._textures[0];
			obj->setTextures(tex);
		}
		if (!loi._skeletons.empty()) {
			const Skeleton& sk = loi._skeletons[0];
			obj->setSkeleton(const_cast<Skeleton&>(sk));
		}
	}
	//for (uint32_t i = 0; i < loi._meshes.size(); i++) {
	//	Object3D* o = new Object3D();
	//	o->setMesh((Mesh*)&loi._meshes[i]);
	//	o->setMaterial((Material*)&loi._materials[i]);
	//}

	//for (const auto& l : loi._children) {
	//	Object3D* o = new Object3D();
	//	Object3D::allocated_objects.push_back(o);
	//	createFromObjectInfo(l, o);
	//	obj->addChild(o);
	//}
}

void Object3D::setMaterial(
	const std::vector<Material>& material,
	std::vector<uint32_t>* indices
) {
	for (const Material& mat : material) {
		_materials.push_back((Material*)&mat);
	}
	_material_indices = indices;
}

std::vector<uint32_t>* Object3D::getMaterialIndices() {
	return _material_indices;
}

void Object3D::setTextures(const std::vector<Texture>& textures) {
	for (const Texture& tex : textures) {
		_texture.push_back((Texture*)&tex);
	}
}

void Object3D::addShader(GraphicShader* shader) {
	if (!_shaders[0]) {
		// remove it
		// if there is already an affected objects (otherwise, segfault)
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

std::unordered_map<Renderer*, std::unordered_map<GraphicShader*, std::unordered_map<Mesh*, std::vector<Object3D*>>>>& Object3D::getMeshObjects() {
	return Object3D::mesh_objects;
}

std::vector<glm::vec3>& Object3D::getCoords(Renderer* renderer, GraphicShader* shader) {
	buildCoords(renderer, shader);
	return Object3D::coords[renderer][shader];
}

std::vector<glm::vec3>& Object3D::updateCoords(Renderer* renderer, GraphicShader* shader) {
	uint32_t offset = 0;
	for (std::pair<Mesh* const, std::vector<Object3D*>>& m_o : Object3D::mesh_objects[renderer][shader]) {
		Mesh* m = m_o.first;
		//std::cout << m->getCoordsSize() << " " << m->getCoordsStride() << std::endl;
		std::memcpy(&Object3D::coords[renderer][shader][0] + offset, &m->getCoords()[0], m->getCoordsSize());

		//for (uint32_t i = 0; i < Object3D::coords[renderer][shader].size(); i++) {
		//	std::cout << i << std::endl;
		//	Object3D::coords[renderer][shader][i].x = 0.0f;
		//	Object3D::coords[renderer][shader][i].y = 0.0f;
		//	Object3D::coords[renderer][shader][i].z = 0.0f;
		//}

		offset += m->getCoordsSize();
	}

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
	return sizeof(*getObjectId(renderer, shader).data());
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

std::vector<glm::uvec4>& Object3D::getJoints(Renderer* renderer, GraphicShader* shader) {
	buildJoints(renderer, shader);
	return Object3D::joints_ids[renderer][shader];
}

uint32_t Object3D::getJointsStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getJoints(renderer, shader).data());
}

uint32_t Object3D::getJointsSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getJointsStride(renderer, shader);
	return getJoints(renderer, shader).size() * stride;
}

std::vector<glm::vec4>& Object3D::getWeights(Renderer* renderer, GraphicShader* shader) {
	buildWeights(renderer, shader);
	return Object3D::weights[renderer][shader];
}

uint32_t Object3D::getWeightsStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getWeights(renderer, shader).data());
}

uint32_t Object3D::getWeightsSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getWeightsStride(renderer, shader);
	return getJoints(renderer, shader).size() * stride;
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
	uint32_t obj_i = 0;
	// For each mesh in the renderer
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		std::vector<Object3D*> objects = m_o.second;
		uint32_t n = objects.size();

		for (uint32_t i = 0; i < n; i++) {
			Object3D* obj = objects[i];
			Object3D::transform_matrices[renderer][shader][obj_i] = obj->getTransform();
			obj_i++;
		}
	}

	return Object3D::transform_matrices[renderer][shader];
}

uint32_t Object3D::getTransformMatricesSize(Renderer* renderer, GraphicShader* shader) {
	return getTransformMatrices(renderer, shader).size() * sizeof(*getTransformMatrices(renderer, shader).data());
}

std::vector<uint32_t>& Object3D::getMaterialIndices(Renderer* renderer, GraphicShader* shader) {
	buildMaterialIndices(renderer, shader);
	return Object3D::material_indices[renderer][shader];
}

uint32_t Object3D::getMaterialIndicesStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getMaterialIndices(renderer, shader).data());
}

uint32_t Object3D::getMaterialIndicesSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getMaterialIndicesStride(renderer, shader);
	return getMaterialIndices(renderer, shader).size() * stride;
}

std::vector<glsl::Mat>& Object3D::getMaterials(Renderer* renderer, GraphicShader* shader) {
	buildMaterials(renderer, shader);
	return Object3D::materials[renderer][shader];
}

uint32_t Object3D::getMaterialsStride(Renderer* renderer, GraphicShader* shader) {
	return sizeof(*getMaterials(renderer, shader).data());
}

uint32_t Object3D::getMaterialsSize(Renderer* renderer, GraphicShader* shader) {
	uint32_t stride = getMaterialsStride(renderer, shader);
	return getMaterials(renderer, shader).size() * stride;
}

std::vector<glm::mat4>& Object3D::getJointsTransform(Renderer* renderer, GraphicShader* shader) {
	buildJointsTransform(renderer, shader);
	return Object3D::joints_transform[renderer][shader];
}

std::vector<glm::mat4>& Object3D::updateJointsTransform(Renderer* renderer, GraphicShader* shader) {
	uint32_t joint_i = 0;
	// For each mesh in the renderer
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		std::vector<Object3D*> objs = m_o.second;

		for (Object3D* obj : objs) {
			Skeleton* skeleton = const_cast<Skeleton*>(obj->getSkeleton());
			const std::vector<Joint>& joints = skeleton->joints();

			for (const Joint& joint : joints) {
				const Joint* j = &joint;
				Object3D::joints_transform[renderer][shader][joint_i++] =
					j->getTransform() * j->inverseBindMatrices()
					/*j->initialTransform() * j->getTransformLocale() * j->inverseBindMatrices()*/;
			}
			//i--;
			//joint_i += joints.size();
		}
	}

	return Object3D::joints_transform[renderer][shader];
}

uint32_t Object3D::getJointsTransformSize(Renderer* renderer, GraphicShader* shader) {
	return getJointsTransform(renderer, shader).size() * sizeof(*getJointsTransform(renderer, shader).data());
}

//std::vector<uint32_t>& Object3D::getTextureIndices(Renderer* renderer, GraphicShader* shader) {
//	buildTextureIndices(renderer, shader);
//	return Object3D::texture_indices[renderer][shader];
//}

//uint32_t Object3D::getTextureIndicesStride(Renderer* renderer, GraphicShader* shader) {
//	return sizeof(*getTextureIndices(renderer, shader).data());
//}
//
//uint32_t Object3D::getTextureIndicesSize(Renderer* renderer, GraphicShader* shader) {
//	uint32_t stride = getTextureIndicesStride(renderer, shader);
//	return getTextureIndices(renderer, shader).size() * stride;
//}

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

	uint32_t obj_i{ 0 };
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		Mesh* mesh = m_o.first;
		std::vector<Object3D*>& objects = m_o.second;
		uint32_t nb_coords = mesh->getCoords().size();
		for (uint32_t i = 0; i < nb_coords; i++) {
			Object3D::object_id[renderer][shader].push_back(obj_i);
		}
		obj_i += objects.size();
	}
}

void Object3D::buildIndices(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::indices[renderer][shader].size()) {
		return;
	}

	uint32_t max_i = 0;
	// For each mesh
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		const Mesh* mesh = m_o.first;
		uint32_t tmp = 0;
		for (uint32_t i : mesh->getIndices()) {
			tmp = std::max(tmp, i);
			// We adapt and add the indices
			Object3D::indices[renderer][shader].push_back(i + max_i);
		}
		max_i = max_i + tmp + 1;
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

void Object3D::buildJoints(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::joints_ids[renderer][shader].size()) {
		return;
	}

	uint32_t max_i = 0;
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		const Mesh* mesh = m_o.first;

		uint32_t tmp_i = 0;
		for (glm::uvec4 j_id : mesh->getJoints()) {
			tmp_i = std::max(std::max(j_id.x, j_id.y), std::max(j_id.z, j_id.w));
			Object3D::joints_ids[renderer][shader].push_back(
				glm::uvec4(
					j_id.x + max_i,
					j_id.y + max_i,
					j_id.z + max_i,
					j_id.w + max_i
				)
			);
		}

		max_i = max_i + tmp_i + 1;
	}
}

void Object3D::buildWeights(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::weights[renderer][shader].size()) {
		return;
	}

	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		const Mesh* mesh = m_o.first;
		Object3D::weights[renderer][shader].insert(
			Object3D::weights[renderer][shader].end(),
			mesh->getWeights().begin(),
			mesh->getWeights().end()
		);
	}
}


void Object3D::buildTextures(Renderer* renderer, GraphicShader* shader) {
	buildMaterials(renderer, shader);
	//// if not empty
	//if (Object3D::_textures[renderer][shader].size()) {
	//	return;
	//}
	//_textures[renderer][shader].push_back(DefaultConf::white_texture);

	// For each material


	//std::unordered_map<const Texture*, uint32_t> texes;
	//// Finding the textures
	//for (auto& m_o : mesh_objects[renderer][shader]) {
	//	std::vector<Object3D*> objs = m_o.second;

	//	for (Object3D* obj : objs) {
	//		if (texes.count(obj->getTexture())) {
	//			continue;
	//		}
	//		if (!obj->getTexture()) {
	//			continue;
	//		}
	//		_textures[renderer][shader].push_back(const_cast<Texture*>(obj->getTexture()));
	//		texes[obj->getTexture()] = 1;
	//	}
	//}
}

void Object3D::buildTransformIndices(Renderer* renderer, GraphicShader* shader) {
	// if not empty
	if (Object3D::transform_indices[renderer][shader].size()) {
		return;
	}

	uint32_t tr_i = 0;
	// For each mesh of the pair renderer - shader
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

void Object3D::buildMaterialIndices(Renderer* renderer, GraphicShader* shader) {
	// if not empty, means already done
	if (Object3D::material_indices[renderer][shader].size()) {
		return;
	}

	std::unordered_map<const Material*, uint32_t> is_mat;

	// For each mesh given a renderer and a shader
	uint32_t start_i = 1; // Default material is stored at 0, so we must start at 1
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		Mesh* m = m_o.first;
		std::vector<Object3D*> objs = m_o.second;
		for (auto& obj : objs) {
			std::vector<Material*> mats = obj->getMaterial();

			// When there is only one material
			if (mats.size() <= 1) {
				uint32_t index = start_i;
				if (mats.size() == 0) {
					index = 0;
				}
				else if (is_mat.count(mats[0])) {
					index = is_mat[mats[0]];
				}
				else {
					is_mat[mats[0]] = start_i;
					start_i++;
				}

				std::vector<uint32_t> indices(m->getCoords().size(), index);
				Object3D::material_indices[renderer][shader].insert(
					Object3D::material_indices[renderer][shader].end(),
					indices.begin(),
					indices.end()
				);
				continue;
			}

			// When there are more than one material
			std::vector<uint32_t>* ids = obj->getMaterialIndices();
			uint32_t nb_new = 0;
			for (uint32_t id : *ids) {
				Material* mat = mats[id];
				uint32_t index = start_i + id;
				if (is_mat.count(mat)) {
					index = is_mat[mat];
				}
				else {
					is_mat[mat] = index;
					nb_new++;
				}
				Object3D::material_indices[renderer][shader].push_back(index);
			}
			start_i += nb_new;
			break;
		}
	}
}

void Object3D::buildMaterials(Renderer* renderer, GraphicShader* shader) {
	//buildTextures(renderer, shader);

	// if not empty, means already done
	if (Object3D::materials[renderer][shader].size()) {
		return;
	}

	std::unordered_map<Material*, bool> is_mat{};

	std::unordered_map<Texture*, uint32_t> texes{};
	uint32_t tex_id = 0;

	// If an object exists, then we must add the default material
	// And the default texture
	if (Object3D::mesh_objects[renderer][shader].size()) {
		Material mat{};
		mat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
		mat.map_Kd = 0;
		Object3D::materials[renderer][shader].push_back(glsl::Mat(mat));
		is_mat[(Material*)&mat] = true;
		_textures[renderer][shader].push_back(DefaultConf::white_texture);
		texes[DefaultConf::white_texture] = tex_id;
		tex_id++;
	}

	// For each mesh given a renderer and a shader
	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		Mesh* m = m_o.first;
		std::vector<Object3D*> objs = m_o.second;

		// For each object
		for (auto obj : objs) {
			const std::vector<Texture*>& textures = obj->getTexture();
			for (Material* mat : obj->getMaterial()) {
				// If the material was never added
				// We add it to the array of material
				if (!is_mat.count(mat)) {
					Object3D::materials[renderer][shader].push_back(
						glsl::Mat(*mat)
					);

					if (mat->map_Kd == -1) {
						Object3D::materials[renderer][shader][Object3D::materials[renderer][shader].size() - 1].map_Kd = 0;
						continue;
					}

					Texture* tex = textures[mat->map_Kd];
					if (!texes.count(tex)) {
						texes[tex] = tex_id;
						_textures[renderer][shader].push_back(tex);
						tex_id++;
					}

					Object3D::materials[renderer][shader][Object3D::materials[renderer][shader].size() - 1].map_Kd = texes[tex];
				}
			}
		}
	}
}

void Object3D::buildJointsTransform(Renderer* renderer, GraphicShader* shader) {
	if (Object3D::joints_transform[renderer][shader].size()) {
		return;
	}

	std::unordered_map<const Joint*, bool> is_joint;

	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
		std::vector<Object3D*> objs = m_o.second;
		
		for (Object3D* obj : objs) {
			Skeleton* skeleton = const_cast<Skeleton*>(obj->getSkeleton());
			const std::vector<Joint>& joints = skeleton->joints();
			for (const Joint& joint : joints) {
				const Joint* j = &joint;
				if (is_joint.count(j)) {
					continue;
				}

				Object3D::joints_transform[renderer][shader].push_back(
					j->getTransform() * j->inverseBindMatrices()
				);
				is_joint[j] = true;
			}
		}
	}
}

//void Object3D::buildTextureIndices(Renderer* renderer, GraphicShader* shader) {
//	// if not empty
//	if (Object3D::texture_indices[renderer][shader].size()) {
//		return;
//	}
//
//	uint32_t tex_i = 0;
//	std::unordered_map<const Texture*, uint32_t> tex_i_arr{};
//	// For each mesh, finding the associated objects
//	for (auto& m_o : Object3D::mesh_objects[renderer][shader]) {
//		if (!m_o.first) {
//			continue;
//		}
//		std::vector<Object3D*> objs = m_o.second;
//
//		// For each object add find the texture index
//		for (Object3D* obj : objs) {
//			const Texture* tex = obj->getTexture();
//			if (!tex_i_arr.count(tex)) {
//				tex_i = tex_i_arr.size();
//				tex_i_arr[tex] = tex_i;
//			}
//			else {
//				tex_i = tex_i_arr[tex];
//			}
//
//			// +1 because index 0 is the default texture
//			texture_indices[renderer][shader].push_back(tex_i + 1);
//		}
//	}
//}

void Object3D::freeAllocatedObjects() {
	for (Object3D* obj : Object3D::allocated_objects) {
		delete obj;
	}
}