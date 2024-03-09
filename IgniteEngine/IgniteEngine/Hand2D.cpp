#include "Hand2D.h"

Hand2D::Hand2D() {
	;
}

void Hand2D::init() {
	Module::init();

	const uint32_t nb_elem_max = 100000;

	DefaultConf::coord_buffer->setCapacity(nb_elem_max * sizeof(glm::vec3));
	DefaultConf::index_buffer->setCapacity(nb_elem_max * sizeof(uint32_t));
	DefaultConf::uv_buffer->setCapacity(nb_elem_max * sizeof(glm::vec3));

	std::cout << "Total coord capacity: " << nb_elem_max * sizeof(glm::vec3) << std::endl;
	std::cout << "Total index capacity: " << nb_elem_max * sizeof(uint32_t) << std::endl;
}

void rectangle(Mesh& mesh) {
	std::vector<glm::vec3> coords;
	std::vector<uint32_t> indices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> n;

	coords.push_back(glm::vec3(-1.0f, -1.0f, 0.0));
	coords.push_back(glm::vec3(1.0f, -1.0f, 0.0));
	coords.push_back(glm::vec3(1.0f, 1.0f, 0.0));
	coords.push_back(glm::vec3(-1.0f, 1.0f, 0.0));

	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));

	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	mesh.setVertex(coords, indices, uv, n);
}

void circle(Mesh& mesh, float r, uint32_t precision) {
	std::vector<glm::vec3> coords;
	std::vector<uint32_t> indices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> n;

	float step = 2 * M_PI / precision;
	
	coords.push_back(glm::vec3(0.0f, 0.0f, 0.0));
	coords.push_back(glm::vec3(1.0f, 0.0f, 0.0));
	uv.push_back(glm::vec2(0.0F, 0.0F));
	uv.push_back(glm::vec2(0.0F, 0.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
	
	uint32_t index = 2;

	for (uint32_t i = 1; i < precision; i++) {
		float x = r * std::cos(step * i);
		float y = r * std::sin(step * i);
	
		coords.push_back(glm::vec3(x, y, 0.0f));
		uv.push_back(glm::vec2(0.0f, 0.0f));
		n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
		indices.push_back(0);
		indices.push_back(index-1);
		indices.push_back(index);

		index++;
	}

	indices.push_back(0);
	indices.push_back(precision);
	indices.push_back(1);

	mesh.setVertex(coords, indices, uv, n);
}

void addVertices(Mesh& mesh, float distance) {
	std::vector<glm::vec3> coords;
	std::vector<uint32_t> indices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> n;

	std::vector<glm::vec3> tmp_coords = mesh.getCoords();
	std::vector<uint32_t> tmp_indices = mesh.getIndices();
	std::vector<glm::vec2> tmp_uv = mesh.getUV();
	std::vector<glm::vec3> tmp_n = mesh.getNormals();

	coords.push_back(tmp_coords[0]);
	coords.push_back(tmp_coords[1]);
	uv.push_back(tmp_uv[0]);
	uv.push_back(tmp_uv[1]);
	n.push_back(tmp_n[0]);
	n.push_back(tmp_n[1]);
	
	uint32_t index = 2;
	for (uint32_t i = 1; i < tmp_coords.size() - 1; ++i) {
		glm::vec3 cur = tmp_coords[i];
		glm::vec3 next = tmp_coords[i + 1];

		glm::vec3 cn = next - cur;
		if (glm::length(cn) > distance) {
			cn = glm::normalize(cn);
			glm::vec3 new_c = cur + cn * 0.5f;

			coords.push_back(new_c);
			uv.push_back(glm::vec2(0.0f, 0.0f));
			n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
			indices.push_back(0);
			indices.push_back(index - 1);
			indices.push_back(index);
			index++;
		}
		
		coords.push_back(next);
		uv.push_back(glm::vec2(0.0f, 0.0f));
		n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
		indices.push_back(0);
		indices.push_back(index - 1);
		indices.push_back(index);
		
		index++;
	}

	indices.push_back(0);
	indices.push_back(index - 1);
	indices.push_back(1);

	mesh.setVertex(coords, indices, uv, n);
}

void Hand2D::start() {
	Module::start();

	//rectangle(_circle_mesh);
	circle(_circle_mesh, 1.0F, 5);
	_circle_obj.setMesh(&_circle_mesh);
	_circle_obj.setRenderer(DefaultConf::renderer);
	_circle_obj.addShader(DefaultConf::graphic_shader);

	_hand_img.readFile("../assets/hand_segmented.png");
	_center_x = _hand_img.getWidth() / 2;
	_center_y = _hand_img.getHeight() / 2;

	{
		glm::vec4 pixel = _hand_img.getPixel(0, 0);
		std::cout << pixel.r << " " << pixel.g << " " << pixel.b << " " << pixel.a << std::endl;
	}

	{
		/*glm::vec4& pixel = _hand_img.getPixel(_hand_img.getWidth() / 2, _hand_img.getHeight() / 2);*/
		glm::vec4 pixel = _hand_img.getPixel(171, 922);
		std::cout << pixel.r << " " << pixel.g << " " << pixel.b << " " << pixel.a << std::endl;
	}

	std::cout << -1 % 10 << std::endl;
}

void Hand2D::update() {
	Module::update();

	if (DefaultConf::event->type == SDL_KEYDOWN) {
		if (DefaultConf::event->key.keysym.sym == SDLK_m) {
			_modify_vertex = true;
		}
		if (DefaultConf::event->key.keysym.sym == SDLK_l) {
			_modify_buffer = true;
		}
	}

	//if (_modify_buffer_test) {
	//	circle(_circle_mesh, 1.0F, 20);
	//	DefaultConf::coord_buffer->setSize(_circle_mesh.getCoordsSize());
	//	DefaultConf::index_buffer->setSize(_circle_mesh.getIndicesSize());

	//	std::vector<glm::vec3> coords = _circle_mesh.getCoords();
	//	DefaultConf::coord_buffer->setValues(
	//		&coords[0]
	//	);
	//	//DefaultConf::coord_buffer->setValues(
	//	//	&Object3D::updateCoords(DefaultConf::renderer, DefaultConf::graphic_shader)[0]
	//	//);

	//	std::vector<uint32_t> indices = _circle_mesh.getIndices();
	//	DefaultConf::index_buffer->setValues(
	//		&indices[0]
	//	);

	//	_modify_vertex = false;
	//	_modify_buffer_test = false;
	//}

	if (_modify_buffer) {
		addVertices(_circle_mesh, 0.1);

		std::cout << "coords size:  " << _circle_mesh.getCoordsSize() << std::endl;
		std::cout << "indices size: " << _circle_mesh.getIndicesSize() << std::endl;

		DefaultConf::coord_buffer->setSize(_circle_mesh.getCoordsSize());
		DefaultConf::index_buffer->setSize(_circle_mesh.getIndicesSize());

		std::vector<glm::vec3> coords = _circle_mesh.getCoords();
		DefaultConf::coord_buffer->setValues(
			&coords[0]
		);

		std::vector<uint32_t> indices = _circle_mesh.getIndices();
		DefaultConf::index_buffer->setValues(
			&indices[0]
		);

		_modify_vertex = false;
		_modify_buffer = false;
	}

	if (_modify_vertex) {
		//std::cout << "modify" << std::endl;
		std::vector<glm::vec3>& c = const_cast<std::vector<glm::vec3>&>(_circle_mesh.getCoords());
		std::vector<glm::vec3> cc = c;

		for (uint32_t i = 1; i < _circle_mesh.getCoords().size(); i++) {
			int prev_i = i - 1;
			int next_i = i + 1;
			if (prev_i == 0) {
				prev_i = _circle_mesh.getCoords().size() - 1;
			}
			if (next_i == _circle_mesh.getCoords().size()) {
				next_i = 1;
			}

			glm::vec3 d = cc[next_i] - cc[prev_i];
			glm::vec3 normal = glm::normalize(glm::vec3(d.y, -d.x, d.z));


			uint32_t pix_x = cc[i].x * _world_to_pix + _center_x;
			uint32_t pix_y = cc[i].y * _world_to_pix + _center_y;


			if (pix_x < 0 || pix_x >= _hand_img.getWidth() || pix_y < 0 || pix_y >= _hand_img.getHeight()) {
				continue;
			}
			glm::vec4 pix = _hand_img.getPixel(pix_y, pix_x);

			bool is_negatif{ false };
			if (pix.a == 0) {
				normal = -normal;
				is_negatif = true;
			}

			pix_x += normal.x * _pix_to_world;
			pix_y += normal.y * _pix_to_world;

			if (pix_x < 0 || pix_x >= _hand_img.getWidth() || pix_y < 0 || pix_y >= _hand_img.getHeight()) {
				continue;
			}

			pix = _hand_img.getPixel(pix_y, pix_x);
			if (pix.a == 0 && !is_negatif) {
				continue;
			}
			else if (pix.a != 0 && is_negatif) {
				continue;
			}

			c[i] = glm::vec3(cc[i].x + normal.x * _pix_to_world, cc[i].y + normal.y * _pix_to_world, cc[i].z);
		}

		//DefaultConf::coord_buffer->setValues(
		//	&Object3D::updateCoords(DefaultConf::renderer, DefaultConf::graphic_shader)[0]
		//);

		std::vector<glm::vec3> coords = _circle_mesh.getCoords();
		DefaultConf::coord_buffer->setValues(
			&coords[0]
		);

		//_modify_vertex = !_modify_vertex;
	}
}

void Hand2D::close() {
	Module::close();

}