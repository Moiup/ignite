#include "LoadedObjectInfo.h"

#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

LoadedObjectInfo::LoadedObjectInfo() { ; }

void LoadedObjectInfo::loadWavefont(const std::string& file_name) {
	fastObjMesh* fom = fast_obj_read(file_name.c_str());
	if (!fom) {
		std::cout << "Error opening the file: " << file_name << std::endl;
		throw std::runtime_error("Error opening file.");
	}

	std::cout << "Loading obj file: ";
	std::cout << file_name << std::endl;

	_materials.push_back(std::vector<Material>());
	_textures.push_back(std::vector<Texture>());
	std::unordered_map<char*, uint32_t> mat_to_tex;
	uint32_t t_id{0};
	for (uint32_t i = 0; i < fom->material_count; i++) {
		_materials[0].push_back(Material(fom->materials[i]));
		if (!fom->materials[i].map_Kd.path) {
			continue;
		}

		std::cout << fom->materials[i].map_Kd.path << std::endl;
		if (!mat_to_tex.count(fom->materials[i].map_Kd.path)) {
			mat_to_tex[fom->materials[i].map_Kd.path] = t_id;
			Pixels pixels(fom->materials[i].map_Kd.path);
			_textures[0].push_back(Texture());
			_textures[0][t_id].setQueue(DefaultConf::graphics_queue);
			_textures[0][t_id].setDimensions(pixels.getWidth(), pixels.getHeight());
			_textures[0][t_id].create();
			_textures[0][t_id].update(pixels);
			_textures[0][t_id].changeLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			t_id++;
		}
		_materials[0][i].map_Kd = mat_to_tex[fom->materials[i].map_Kd.path];
	}

	DefaultConf::graphics_queue->submit();
	DefaultConf::graphics_queue->wait();

	_meshes.push_back(Mesh());
	std::vector<uint32_t> mat_id{};

	std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>> is_vertex;

	{
		uint32_t f = 0;
		uint32_t v_i = 0;
		while (f < fom->face_count) {
			buildFace(fom, _meshes[0]._indices, mat_id, _meshes[0]._coords, _meshes[0]._normals, _meshes[0]._uv, is_vertex, f, v_i);
			buildFace(fom, _meshes[0]._indices, mat_id, _meshes[0]._coords, _meshes[0]._normals, _meshes[0]._uv, is_vertex, f, v_i + 1);
			buildFace(fom, _meshes[0]._indices, mat_id, _meshes[0]._coords, _meshes[0]._normals, _meshes[0]._uv, is_vertex, f, v_i + 2);

			if (fom->face_vertices[f] == 4) {
				buildFace(fom, _meshes[0]._indices, mat_id, _meshes[0]._coords, _meshes[0]._normals, _meshes[0]._uv, is_vertex, f, v_i);
				buildFace(fom, _meshes[0]._indices, mat_id, _meshes[0]._coords, _meshes[0]._normals, _meshes[0]._uv, is_vertex, f, v_i + 2);
				buildFace(fom, _meshes[0]._indices, mat_id, _meshes[0]._coords, _meshes[0]._normals, _meshes[0]._uv, is_vertex, f, v_i + 3);
			}

			v_i += fom->face_vertices[f];
			f++;
		}
	}

	_material_indices.push_back(std::vector<uint32_t>());
	_material_indices[0].assign(mat_id.data(), mat_id.data() + mat_id.size());
	
	fast_obj_destroy(fom);
	std::cout << " loaded!" << std::endl;
}

void LoadedObjectInfo::loadGLTF(const std::string& file_name) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;

	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, file_name);

	if (!warn.empty()) {
		std::cout << "Warning while opening the '" << file_name << "':" << warn << std::endl;
	}
	if (!err.empty()) {
		std::cout << "Error while opening the '" << file_name << "':" << err << std::endl;
	}

	if (!ret) {
		std::cout << "Error opening the file: " << file_name << std::endl;
	}

	std::cout << "Loading gLTF file: ";
	std::cout << file_name << std::endl;

	// For now, we consider only one mesh
	_meshes.push_back(Mesh());
	
	tinygltf::Mesh& mesh = model.meshes[0];
	std::map<std::string, int>& attributes = mesh.primitives[0].attributes;

	// ---- Vertex Attributes ----
	// Positions
	GLTFBuffInfo pos_buf_info = retreiveBufferDataGltf(model, attributes["POSITIONS"]);
	_meshes[0].setCoords(
		reinterpret_cast<float*>(pos_buf_info._data),
		pos_buf_info._byte_length / (sizeof(glm::vec3))
	);

	// Normals
	GLTFBuffInfo normal_buf_info = retreiveBufferDataGltf(model, attributes["NORMAL"]);
	_meshes[0].setNormals(
		reinterpret_cast<float*>(normal_buf_info._data),
		normal_buf_info._byte_length / (sizeof(glm::vec3))
	);
	// UV coordinates
	GLTFBuffInfo uv_buf_info = retreiveBufferDataGltf(model, attributes["TEXCOORD_0"]);
	_meshes[0].setUV(
		reinterpret_cast<float*>(uv_buf_info._data),
		uv_buf_info._byte_length / (sizeof(glm::vec2))
	);
	// Indices (vertex)
	GLTFBuffInfo indices_buf_info = retreiveBufferDataGltf(model, mesh.primitives[0].indices);
	uint32_t* indices{ nullptr };
	std::vector<uint32_t> indices_tmp;

	switch (indices_buf_info._component_type) {
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		{
			indices = reinterpret_cast<uint32_t*>(indices_buf_info._data);
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		{
			uint8_t* indices_ptr = reinterpret_cast<uint8_t*>(indices_buf_info._data);
			indices_tmp.resize(indices_buf_info._count);
			for (uint32_t i = 0; i < indices_buf_info._count; ++i) {
				indices_tmp[i] = indices_ptr[i];
			}
			indices = indices_tmp.data();

			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: 
		{
			uint16_t* indices_ptr = reinterpret_cast<uint16_t*>(indices_buf_info._data);
			indices_tmp.resize(indices_buf_info._count);
			for (uint32_t i = 0; i < indices_buf_info._count; ++i) {
				indices_tmp[i] = indices_ptr[i];
			}
			indices = indices_tmp.data();

			break;
		}
	}
	if (indices) {
		_meshes[0].setIndices(
			indices,
			indices_buf_info._count
		);
	}
	
	// ---- Skinning ----
	if (attributes["JOINTS_0"]) {
		std::cout << "Building skeleton" << std::endl;
		// ---- Vertex Attributes ----
		GLTFBuffInfo joints_buf_info = retreiveBufferDataGltf(model, attributes["JOINTS_0"]);

		std::vector<uint32_t> joints_data(joints_buf_info._count * 4);
		// Must be adapted regarding the component_type of the buffer
		
		switch (joints_buf_info._component_type) {
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
			for (uint32_t i = 0; i < joints_data.size(); ++i) {
				joints_data[i] = joints_buf_info._data[i];
			}
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_SHORT: {
			uint16_t* tmp = reinterpret_cast<uint16_t*>(joints_buf_info._data);
			for (uint32_t i = 0; i < joints_data.size(); ++i) {
				joints_data[i] = tmp[i];
			}
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
			joints_data.assign(joints_buf_info._data, joints_buf_info._data + joints_data.size());
			break;
		}
		}

		_meshes[0].setJoints(
			joints_data.data(),
			joints_buf_info._count
		);

		GLTFBuffInfo weights_buf_info = retreiveBufferDataGltf(model, attributes["WEIGHTS_0"]);
		_meshes[0].setWeights(
			reinterpret_cast<float*>(weights_buf_info._data),
			weights_buf_info._byte_length / (sizeof(glm::vec4))
		);

		tinygltf::Skin& skin = model.skins[0];
		
		// Retreiving the buffer of inverse Bind Matrices
		GLTFBuffInfo inv_bind_mat_buf_info = retreiveBufferDataGltf(model, skin.inverseBindMatrices);
		glm::mat4* inverse_bind_matrices = reinterpret_cast<glm::mat4*>(inv_bind_mat_buf_info._data);

		_skeletons.push_back(Skeleton());
		_joints.push_back(std::vector<Joint>());
		Skeleton& skeleton = _skeletons[0];
		std::vector<Joint>& joints = skeleton.joints();
		joints.resize(skin.joints.size());

		std::vector<tinygltf::Node>& nodes = model.nodes;
		std::unordered_map<int32_t, int32_t> node_to_joint;
		for (int32_t i = 0; i < skin.joints.size(); i++) {
			node_to_joint[skin.joints[i]] = i;
			std::cout << "node_to_joint: " << skin.joints[i] << " - " << i << std::endl;
		}

		for (int32_t joint_i = 0; joint_i < joints.size(); ++joint_i) {
			Joint& joint = joints[joint_i];
			tinygltf::Node& node = nodes[skin.joints[joint_i]];
			for (int32_t child_i : node.children) {
				joint.addChild(&joints[node_to_joint[child_i]]);
			}
			joint.setId(joint_i);
			joint.name() = node.name;

			if (!node.translation.empty()) {
				joint.setPositionLocale(
					node.translation[0],
					node.translation[1],
					node.translation[2]
				);
			}

			if (inv_bind_mat_buf_info._count) {
				joint.inverseBindMatrices() = inverse_bind_matrices[joint_i];
			}
		}
		// Adding the father node as the skeleton start
		skeleton.setSkeleton(&skeleton.joints()[0]);
	}
	std::cout << "loaded." << std::endl;
}

void LoadedObjectInfo::buildFace(
	fastObjMesh* fom,
	std::vector<uint32_t>& indices,
	std::vector<uint32_t>& mat_id,
	std::vector<glm::vec3>& v_coord,
	std::vector<glm::vec3>& n_coord,
	std::vector<glm::vec2>& t_coord,
	std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::unordered_map<uint32_t, uint32_t>>>& is_vertex,
	uint32_t f,
	uint32_t v_i
) {
	uint32_t p_i = fom->indices[v_i].p;
	uint32_t n_i = fom->indices[v_i].n;
	uint32_t t_i = fom->indices[v_i].t;

	if (!is_vertex.count(p_i)) {
		is_vertex[p_i][n_i][t_i] = v_coord.size();
		v_coord.push_back({
			fom->positions[3 * p_i],
			fom->positions[3 * p_i + 1],
			fom->positions[3 * p_i + 2]
			});
		n_coord.push_back({
			fom->normals[3 * n_i],
			fom->normals[3 * n_i + 1],
			fom->normals[3 * n_i + 2]
			});
		t_coord.push_back({
			fom->texcoords[2 * t_i],
			fom->texcoords[2 * t_i + 1]
			});
		mat_id.push_back(fom->face_materials[f]);
	}
	else if (!is_vertex[p_i].count(n_i)) {
		is_vertex[p_i][n_i][t_i] = v_coord.size();
		v_coord.push_back({
			fom->positions[3 * p_i],
			fom->positions[3 * p_i + 1],
			fom->positions[3 * p_i + 2]
			});
		n_coord.push_back({
			fom->normals[3 * n_i],
			fom->normals[3 * n_i + 1],
			fom->normals[3 * n_i + 2]
			});
		t_coord.push_back({
			fom->texcoords[2 * t_i],
			fom->texcoords[2 * t_i + 1]
			});
		mat_id.push_back(fom->face_materials[f]);
	}
	else if (!is_vertex[p_i][n_i].count(t_i)) {
		is_vertex[p_i][n_i][t_i] = v_coord.size();
		v_coord.push_back({
			fom->positions[3 * p_i],
			fom->positions[3 * p_i + 1],
			fom->positions[3 * p_i + 2]
			});
		n_coord.push_back({
			fom->normals[3 * n_i],
			fom->normals[3 * n_i + 1],
			fom->normals[3 * n_i + 2]
			});
		t_coord.push_back({
			fom->texcoords[2 * t_i],
			fom->texcoords[2 * t_i + 1]
			});
		mat_id.push_back(fom->face_materials[f]);
	}
	indices.push_back(is_vertex[p_i][n_i][t_i]);
}

GLTFBuffInfo LoadedObjectInfo::retreiveBufferDataGltf(const tinygltf::Model& model, int accessor_index) {
	const std::vector<tinygltf::Accessor>& accessors = model.accessors;
	const std::vector<tinygltf::BufferView>& buffer_views = model.bufferViews;
	const std::vector<tinygltf::Buffer>& buffers = model.buffers;

	const tinygltf::Accessor& accessor = accessors[accessor_index];
	const tinygltf::BufferView& buffer_view = buffer_views[accessor.bufferView];
	const tinygltf::Buffer& buffer = buffers[buffer_view.buffer];

	return {
		const_cast<unsigned char*>(buffer.data.data() + buffer_view.byteOffset),
		buffer_view.byteLength,
		accessor.count,
		accessor.componentType
	};
}