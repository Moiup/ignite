#include "Object3D.h"

std::unordered_map<Renderer*, std::unordered_map<GraphicsPipeline*, Object3DArrays>> Object3D::_arrays;
std::vector<Object3D*> Object3D::allocated_objects{};

Object3D::Object3D() :
	_mesh{ nullptr },
	_skeleton{ nullptr },
	_renderer{ nullptr },
	_material_indices{},
	_materials{},
	_gps{ nullptr },
	_Texture2D{}
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
	this->_Texture2D = o._Texture2D;
	this->_gps = o._gps;
}

void Object3D::setMesh(Mesh& mesh) {
	uint32_t i = 0;
	// For the previous mesh
	for (GraphicsPipeline* gp : _gps) {
		for (Object3D* obj : Object3D::_arrays[_renderer][gp].mesh_objects[_mesh]) {
			// Find the object
			if (obj != this) {
				i++;
				continue;
			}
			// Remove it from the vector associated to this mesh
			Object3D::_arrays[_renderer][gp].mesh_objects[_mesh].erase(
				Object3D::_arrays[_renderer][gp].mesh_objects[_mesh].begin() + i
			);
		}
		Object3D::_arrays[_renderer][gp].mesh_objects[&mesh].push_back(this);
	}
	_mesh = &mesh;
}

const Mesh& Object3D::getMesh() const {
	return *_mesh;
}

void Object3D::setSkeleton(Skeleton& skeleton) {
	_skeleton = &skeleton;
}

Skeleton* Object3D::getSkeleton() {
	return _skeleton;
}

const Skeleton* Object3D::getSkeleton() const {
	return _skeleton;
}

void Object3D::setRenderer(Renderer& renderer) {
	uint32_t i = 0;
	// For the previous renderer
	// In the corresponding mesh
	for (auto gp : _gps) {
		for (Object3D* obj : Object3D::_arrays[_renderer][gp].mesh_objects[_mesh]) {
			// Find the object
			if (obj != this) {
				i++;
				continue;
			}
			// Remove it from the vector associated to this mesh
			Object3D::_arrays[_renderer][gp].mesh_objects[_mesh].erase(
				Object3D::_arrays[_renderer][gp].mesh_objects[_mesh].begin() + i
			);
		}
		// Finally, update the renderer and put the object in the right places
		Object3D::_arrays[&renderer][gp].mesh_objects[_mesh].push_back(this);
	}
	_renderer = &renderer;
}

void Object3D::setMaterial(Material& material) {
	_materials = { &material };
	_material_indices = { 0 };
}

const std::vector<Material*>& Object3D::getMaterial() const {
	return _materials;
}

void Object3D::setTexture2D(std::vector<Texture2D*>& Texture2D) {
	_Texture2D = Texture2D;
}

const std::vector<Texture2D*>& Object3D::getTexture2D() const {
	return _Texture2D;
}

void Object3D::createFromObjectInfo(const LoadedObjectInfo& loi) {
	createFromObjectInfo(loi, this);
}

void Object3D::createFromObjectInfo(const LoadedObjectInfo& loi, Object3D* obj) {
	if (loi._meshes.size() == 1) {
		obj->setMesh(const_cast<Mesh&>(loi._meshes[0]));
		if (!loi._materials.empty()) {
			const std::vector<Material>& mat = loi._materials[0];
			obj->setMaterial(
				mat,
				(std::vector<uint32_t>*) & loi._material_indices[0]
			);
		}
		if (!loi._textures.empty()) {
			std::vector<Texture2D>& tex = (std::vector<Texture2D>&)loi._textures[0];
			obj->setTextures2D(tex);
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

void Object3D::setTextures2D(const std::vector<Texture2D>& Textures2D) {
	for (const Texture2D& tex : Textures2D) {
		_Texture2D.push_back((Texture2D*)&tex);
	}
}

void Object3D::addGraphicsPipeline(GraphicsPipeline& gp) {
	if (!_gps[0]) {
		// remove it
		// if there is already an affected objects (otherwise, segfault)
		if (Object3D::_arrays[_renderer][nullptr].mesh_objects[_mesh].size()) {
			Object3D::_arrays[_renderer][nullptr].mesh_objects[_mesh].erase(
				Object3D::_arrays[_renderer][_gps[0]].mesh_objects[_mesh].begin()
			);
		}
	}
	_gps.push_back(&gp);

	Object3D::_arrays[_renderer][&gp].mesh_objects[_mesh].push_back(this);
}

const GraphicsPipeline& Object3D::getGraphicsPipeline(uint32_t i) const {
	return *_gps[i];
}

const std::vector<GraphicsPipeline*>& Object3D::getGraphicsPipeline() const {
	return _gps;
}

const Renderer* Object3D::getRenderer() const {
	return _renderer;
}

const std::unordered_map<Renderer*, std::unordered_map<GraphicsPipeline*, Object3DArrays>>& Object3D::getArrays() {
	return Object3D::_arrays;
}

const std::unordered_map<GraphicsPipeline*, Object3DArrays>& Object3D::getArrays(Renderer& renderer) {
	return Object3D::_arrays.at(&renderer);
}

const std::vector<glm::vec3>& Object3D::getCoords(Renderer& renderer, GraphicsPipeline& gp) {
	buildCoords(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].coords;
}

std::vector<glm::vec3>& Object3D::updateCoords(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t offset = 0;
	for (std::pair<Mesh* const, std::vector<Object3D*>>& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		Mesh* m = m_o.first;
		//std::cout << m->getCoordsSize() << " " << m->getCoordsStride() << std::endl;
		std::memcpy(&Object3D::_arrays[&renderer][&gp].coords[0] + offset, &m->getCoords()[0], m->getCoordsSize());

		//for (uint32_t i = 0; i < Object3D::coords[&renderer][&gp].size(); i++) {
		//	std::cout << i << std::endl;
		//	Object3D::coords[&renderer][&gp][i].x = 0.0f;
		//	Object3D::coords[&renderer][&gp][i].y = 0.0f;
		//	Object3D::coords[&renderer][&gp][i].z = 0.0f;
		//}

		offset += m->getCoordsSize();
	}

	return Object3D::_arrays[&renderer][&gp].coords;
}

uint32_t Object3D::getCoordsStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getCoords(renderer, gp).data());
}

uint32_t Object3D::getCoordsSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getCoordsStride(renderer, gp);
	return getCoords(renderer, gp).size() * stride;
}

std::vector<uint32_t>& Object3D::getObjectId(Renderer& renderer, GraphicsPipeline& gp) {
	buildObjectId(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].object_id;
}

uint32_t Object3D::getObjectIdStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getObjectId(renderer, gp).data());
}

uint32_t Object3D::getObjectIdSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getObjectIdStride(renderer, gp);
	return getObjectId(renderer, gp).size() * stride;
}

std::vector<uint32_t>& Object3D::getMeshOffsets(Renderer& renderer, GraphicsPipeline& gp) {
	buildMeshOffsets(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].mesh_offsets;
}

uint32_t Object3D::getMeshOffsetsStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getMeshOffsets(renderer, gp).data());
}

uint32_t Object3D::getMeshOffsetsSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getMeshOffsetsStride(renderer, gp);
	return getMeshOffsets(renderer, gp).size() * stride;
}

std::vector<uint32_t>& Object3D::getIndices(Renderer& renderer, GraphicsPipeline& gp) {
	buildIndices(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].indices;
}

uint32_t Object3D::getIndicesSize(Renderer& renderer, GraphicsPipeline& gp) {
	return getIndicesNbElem(renderer, gp) * sizeof(*getIndices(renderer, gp).data());
}

uint32_t Object3D::getIndicesNbElem(Renderer& renderer, GraphicsPipeline& gp) {
	return getIndices(renderer, gp).size();
}

std::vector<glm::vec2>& Object3D::getUV(Renderer& renderer, GraphicsPipeline& gp) {
	buildUV(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].uv;
}

uint32_t Object3D::getUVStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getUV(renderer, gp).data());
}

uint32_t Object3D::getUVSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getUVStride(renderer, gp);
	return getUV(renderer, gp).size() * stride;
}

std::vector<glm::uvec4>& Object3D::getJoints(Renderer& renderer, GraphicsPipeline& gp) {
	buildJoints(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].joints_ids;
}

uint32_t Object3D::getJointsStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getJoints(renderer, gp).data());
}

uint32_t Object3D::getJointsSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getJointsStride(renderer, gp);
	return getJoints(renderer, gp).size() * stride;
}

std::vector<glm::vec4>& Object3D::getWeights(Renderer& renderer, GraphicsPipeline& gp) {
	buildWeights(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].weights;
}

uint32_t Object3D::getWeightsStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getWeights(renderer, gp).data());
}

uint32_t Object3D::getWeightsSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getWeightsStride(renderer, gp);
	return getJoints(renderer, gp).size() * stride;
}

std::vector<Texture2D*>& Object3D::getTextures2D(Renderer& renderer, GraphicsPipeline& gp) {
	buildTextures2D(renderer, gp);
	return Object3D::_arrays[&renderer][&gp]._textures2D;
}

uint32_t Object3D::getTextures2DStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getTextures2D(renderer, gp).data());
}

uint32_t Object3D::getTextures2DSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getTextures2DStride(renderer, gp);
	return getTextures2D(renderer, gp).size() * stride;
}

std::vector<uint32_t>& Object3D::getTransformIndices(Renderer& renderer, GraphicsPipeline& gp) {
	buildTransformIndices(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].transform_indices;
}

uint32_t Object3D::getTransformIndicesSize(Renderer& renderer, GraphicsPipeline& gp) {
	return getTransformIndices(renderer, gp).size() * sizeof(*getTransformIndices(renderer, gp).data());
}

uint32_t Object3D::getTransformIndicesStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getTransformIndices(renderer, gp).data());
}

std::vector<glm::mat4>& Object3D::getTransformMatrices(Renderer& renderer, GraphicsPipeline& gp) {
	buildTransformMatrices(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].transform_matrices;
}

std::vector<glm::mat4>& Object3D::updateTransformMatrices(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t obj_i = 0;
	// For each mesh in the renderer
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		std::vector<Object3D*> objects = m_o.second;
		uint32_t n = objects.size();

		for (uint32_t i = 0; i < n; i++) {
			Object3D* obj = objects[i];
			Object3D::_arrays[&renderer][&gp].transform_matrices[obj_i] = obj->getTransform();
			obj_i++;
		}
	}

	return Object3D::_arrays[&renderer][&gp].transform_matrices;
}

uint32_t Object3D::getTransformMatricesSize(Renderer& renderer, GraphicsPipeline& gp) {
	return getTransformMatrices(renderer, gp).size() * sizeof(*getTransformMatrices(renderer, gp).data());
}

std::vector<uint32_t>& Object3D::getMaterialIndices(Renderer& renderer, GraphicsPipeline& gp) {
	buildMaterialIndices(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].material_indices;
}

uint32_t Object3D::getMaterialIndicesStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getMaterialIndices(renderer, gp).data());
}

uint32_t Object3D::getMaterialIndicesSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getMaterialIndicesStride(renderer, gp);
	return getMaterialIndices(renderer, gp).size() * stride;
}

std::vector<glsl::Mat>& Object3D::getMaterials(Renderer& renderer, GraphicsPipeline& gp) {
	buildMaterials(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].materials;
}

uint32_t Object3D::getMaterialsStride(Renderer& renderer, GraphicsPipeline& gp) {
	return sizeof(*getMaterials(renderer, gp).data());
}

uint32_t Object3D::getMaterialsSize(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t stride = getMaterialsStride(renderer, gp);
	return getMaterials(renderer, gp).size() * stride;
}

std::vector<glm::mat4>& Object3D::getJointsTransform(Renderer& renderer, GraphicsPipeline& gp) {
	buildJointsTransform(renderer, gp);
	return Object3D::_arrays[&renderer][&gp].joints_transform;
}

std::vector<glm::mat4>& Object3D::updateJointsTransform(Renderer& renderer, GraphicsPipeline& gp) {
	uint32_t joint_i = 0;
	// For each mesh in the renderer
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		std::vector<Object3D*> objs = m_o.second;

		for (Object3D* obj : objs) {
			Skeleton* skeleton = const_cast<Skeleton*>(obj->getSkeleton());
			const std::vector<Joint>& joints = skeleton->joints();
			skeleton->skeleton()->buildTransformWorld();
			for (const Joint& joint : joints) {
				const Joint* j = &joint;
				Object3D::_arrays[&renderer][&gp].joints_transform[joint_i++] =
					j->transformWorld() * j->inverseBindMatrices()
					/*j->initialTransform() * j->getTransformLocale() * j->inverseBindMatrices()*/;
				//j->getTransformDeTousLesPeres() * j->matriceRotationTrouve() * j->inverseBindMatrices()
			}
			//i--;
			//joint_i += joints.size();
		}
	}

	return Object3D::_arrays[&renderer][&gp].joints_transform;
}

uint32_t Object3D::getJointsTransformSize(Renderer& renderer, GraphicsPipeline& gp) {
	return getJointsTransform(renderer, gp).size() * sizeof(*getJointsTransform(renderer, gp).data());
}

//std::vector<uint32_t>& Object3D::getTexture2DIndices(Renderer& renderer, GraphicsPipeline& gp) {
//	buildTexture2DIndices(renderer, gp);
//	return Object3D::Texture2D_indices[&renderer][&gp];
//}

//uint32_t Object3D::getTexture2DIndicesStride(Renderer& renderer, GraphicsPipeline& gp) {
//	return sizeof(*getTexture2DIndices(renderer, gp).data());
//}
//
//uint32_t Object3D::getTexture2DIndicesSize(Renderer& renderer, GraphicsPipeline& gp) {
//	uint32_t stride = getTexture2DIndicesStride(renderer, gp);
//	return getTexture2DIndices(renderer, gp).size() * stride;
//}

void Object3D::buildCoords(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].coords.size()) {
		return;
	}

	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		const Mesh* mesh = m_o.first;
		Object3D::_arrays[&renderer][&gp].coords.insert(
			Object3D::_arrays[&renderer][&gp].coords.end(),
			mesh->getCoords().begin(),
			mesh->getCoords().end()
		);
	}
}

void Object3D::buildMeshOffsets(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].mesh_offsets.size()) {
		return;
	}

	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		Mesh* mesh = m_o.first;
		uint32_t mesh_offset = mesh->getIndicesNbElem();
		for (uint32_t i = 0; i < mesh->getCoords().size(); i++) {
			Object3D:_arrays[&renderer][&gp].mesh_offsets.push_back(mesh_offset);
		}
	}
}

void Object3D::buildObjectId(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].object_id.size()) {
		return;
	}
	
	uint32_t obj_i{ 0 };
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		Mesh* mesh = m_o.first;
		std::vector<Object3D*>& objects = m_o.second;
		uint32_t nb_coords = mesh->getCoords().size();
		for (uint32_t i = 0; i < nb_coords; i++) {
			Object3D::_arrays[&renderer][&gp].object_id.push_back(obj_i);
		}
		
		obj_i += objects.size();
	}
}

void Object3D::buildIndices(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].indices.size()) {
		return;
	}

	uint32_t max_i = 0;
	// For each mesh
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		const Mesh* mesh = m_o.first;
		uint32_t tmp = 0;
		for (uint32_t i : mesh->getIndices()) {
			tmp = std::max(tmp, i);
			// We adapt and add the indices
			Object3D::_arrays[&renderer][&gp].indices.push_back(i + max_i);
		}
		max_i = max_i + tmp + 1;
	}
}

void Object3D::buildUV(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].uv.size()) {
		return;
	}

	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		const Mesh* mesh = m_o.first;
		Object3D::_arrays[&renderer][&gp].uv.insert(
			Object3D::_arrays[&renderer][&gp].uv.end(),
			mesh->getUV().begin(),
			mesh->getUV().end()
		);
	}
}

void Object3D::buildJoints(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].joints_ids.size()) {
		return;
	}

	uint32_t max_i = 0;
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		const Mesh* mesh = m_o.first;

		uint32_t tmp_i = 0;
		for (glm::uvec4 j_id : mesh->getJoints()) {
			tmp_i = std::max(std::max(j_id.x, j_id.y), std::max(j_id.z, j_id.w));
			Object3D::_arrays[&renderer][&gp].joints_ids.push_back(
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

void Object3D::buildWeights(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].weights.size()) {
		return;
	}

	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		const Mesh* mesh = m_o.first;
		Object3D::_arrays[&renderer][&gp].weights.insert(
			Object3D::_arrays[&renderer][&gp].weights.end(),
			mesh->getWeights().begin(),
			mesh->getWeights().end()
		);
	}
}


void Object3D::buildTextures2D(Renderer& renderer, GraphicsPipeline& gp) {
	buildMaterials(renderer, gp);
	//// if not empty
	//if (Object3D::_Textures2D[&renderer][&gp].size()) {
	//	return;
	//}
	//_Textures2D[&renderer][&gp].push_back(DefaultConf::white_Texture2D);

	// For each material


	//std::unordered_map<const Texture2D*, uint32_t> texes;
	//// Finding the Textures2D
	//for (auto& m_o : mesh_objects[&renderer][&gp]) {
	//	std::vector<Object3D*> objs = m_o.second;

	//	for (Object3D* obj : objs) {
	//		if (texes.count(obj->getTexture2D())) {
	//			continue;
	//		}
	//		if (!obj->getTexture2D()) {
	//			continue;
	//		}
	//		_Textures2D[&renderer][&gp].push_back(const_cast<Texture2D*>(obj->getTexture2D()));
	//		texes[obj->getTexture2D()] = 1;
	//	}
	//}
}

void Object3D::buildTransformIndices(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].transform_indices.size()) {
		return;
	}

	uint32_t tr_i = 0;
	// For each mesh of the pair renderer - gp
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		if (m_o.first == nullptr) {
			continue;
		}
		// Reach each object
		for (Object3D* obj : m_o.second) {
			const Mesh& mesh = obj->getMesh();
			for (uint32_t i : mesh.getIndices()) {
				Object3D::_arrays[&renderer][&gp].transform_indices.push_back(tr_i);
			}
			tr_i++;
		}
	}
}

void Object3D::buildTransformMatrices(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty
	if (Object3D::_arrays[&renderer][&gp].transform_matrices.size()) {
		return;
	}

	// For each mesh of the renderer
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		// Reach for each object
		for (Object3D* obj : m_o.second) {
			glm::mat4 tr = obj->getTransform();
			Object3D::_arrays[&renderer][&gp].transform_matrices.push_back(tr);
		}
	}
}

void Object3D::buildMaterialIndices(Renderer& renderer, GraphicsPipeline& gp) {
	// if not empty, means already done
	if (Object3D::_arrays[&renderer][&gp].material_indices.size()) {
		return;
	}

	std::unordered_map<const Material*, uint32_t> is_mat;
	std::unordered_map<const Mesh*, bool> is_mesh;

	// For each mesh given a renderer and a gp
	uint32_t start_i = 1; // Default material is stored at 0, so we must start at 1
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		Mesh* m = m_o.first;
		std::vector<Object3D*> objs = m_o.second;
		for (auto& obj : objs) {
			std::vector<Material*> mats = obj->getMaterial();

			if (is_mesh.count(m)) {
				break;
			}
			else {
				is_mesh[m] = true;
			}
			
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
				Object3D::_arrays[&renderer][&gp].material_indices.insert(
					Object3D::_arrays[&renderer][&gp].material_indices.end(),
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
				Object3D::_arrays[&renderer][&gp].material_indices.push_back(index);
			}
			start_i += nb_new;
			break;
		}
	}
}

void Object3D::buildMaterials(Renderer& renderer, GraphicsPipeline& gp) {
	//buildTextures2D(renderer, gp);

	// if not empty, means already done
	if (Object3D::_arrays[&renderer][&gp].materials.size()) {
		return;
	}

	std::unordered_map<Material*, bool> is_mat{};
	std::unordered_map<Mesh*, bool> is_mesh{};

	std::unordered_map<Texture2D*, uint32_t> texes{};
	uint32_t tex_id = 0;

	// If an object exists, then we must add the default material
	// And the default Texture2D
	if (Object3D::_arrays[&renderer][&gp].mesh_objects.size()) {
		Material mat{};
		mat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
		mat.map_Kd = 0;
		Object3D::_arrays[&renderer][&gp].materials.push_back(glsl::Mat(mat));
		is_mat[(Material*)&mat] = true;
		Object3D::_arrays[&renderer][&gp]._textures2D.push_back(DefaultConf::white_texture);
		texes[DefaultConf::white_texture] = tex_id;
		tex_id++;
	}

	// For each mesh given a renderer and a gp
	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		Mesh* m = m_o.first;
		std::vector<Object3D*> objs = m_o.second;

		// For each object
		for (auto obj : objs) {
			const std::vector<Texture2D*>& Textures2D = obj->getTexture2D();

			if (is_mesh.count(m)) {
				break;
			}
			else {
				is_mesh[m] = true;
			}

			for (Material* mat : obj->getMaterial()) {
				// If the material was never added
				// We add it to the array of material
				if (!is_mat.count(mat)) {
					Object3D::_arrays[&renderer][&gp].materials.push_back(
						glsl::Mat(*mat)
					);

					if (mat->map_Kd == -1) {
						Object3D::_arrays[&renderer][&gp].materials[Object3D::_arrays[&renderer][&gp].materials.size() - 1].map_Kd = 0;
						continue;
					}

					Texture2D* tex = Textures2D[mat->map_Kd];
					if (!texes.count(tex)) {
						texes[tex] = tex_id;
						Object3D::_arrays[&renderer][&gp]._textures2D.push_back(tex);
						tex_id++;
					}

					Object3D::_arrays[&renderer][&gp].materials[Object3D::_arrays[&renderer][&gp].materials.size() - 1].map_Kd = texes[tex];
				}
			}
		}
	}
}

void Object3D::buildJointsTransform(Renderer& renderer, GraphicsPipeline& gp) {
	if (Object3D::_arrays[&renderer][&gp].joints_transform.size()) {
		return;
	}

	std::unordered_map<const Joint*, bool> is_joint;

	for (auto& m_o : Object3D::_arrays[&renderer][&gp].mesh_objects) {
		std::vector<Object3D*> objs = m_o.second;
		
		for (Object3D* obj : objs) {
			Skeleton* skeleton = const_cast<Skeleton*>(obj->getSkeleton());
			skeleton->skeleton()->buildTransformWorld();
			const std::vector<Joint>& joints = skeleton->joints();
			for (const Joint& joint : joints) {
				const Joint* j = &joint;
				if (is_joint.count(j)) {
					continue;
				}

				Object3D::_arrays[&renderer][&gp].joints_transform.push_back(
					j->transformWorld() * j->inverseBindMatrices()
				);
				is_joint[j] = true;
			}
		}
	}
}

//void Object3D::buildTexture2DIndices(Renderer& renderer, GraphicsPipeline& gp) {
//	// if not empty
//	if (Object3D::Texture2D_indices[&renderer][&gp].size()) {
//		return;
//	}
//
//	uint32_t tex_i = 0;
//	std::unordered_map<const Texture2D*, uint32_t> tex_i_arr{};
//	// For each mesh, finding the associated objects
//	for (auto& m_o : Object3D::_arrays[&renderer][&gp]) {
//		if (!m_o.first) {
//			continue;
//		}
//		std::vector<Object3D*> objs = m_o.second;
//
//		// For each object add find the Texture2D index
//		for (Object3D* obj : objs) {
//			const Texture2D* tex = obj->getTexture2D();
//			if (!tex_i_arr.count(tex)) {
//				tex_i = tex_i_arr.size();
//				tex_i_arr[tex] = tex_i;
//			}
//			else {
//				tex_i = tex_i_arr[tex];
//			}
//
//			// +1 because index 0 is the default Texture2D
//			Texture2D_indices[&renderer][&gp].push_back(tex_i + 1);
//		}
//	}
//}

void Object3D::freeAllocatedObjects() {
	for (Object3D* obj : Object3D::allocated_objects) {
		delete obj;
	}
}