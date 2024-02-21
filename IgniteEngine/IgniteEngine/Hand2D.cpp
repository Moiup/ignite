#include "Hand2D.h"

Hand2D::Hand2D() {
	;
}

void Hand2D::init() {
	Module::init();

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

	for (uint32_t i = 1; i <= precision; i++) {
		float x = r * std::cos(step * i);
		float y = r * std::sin(step * i);
	
		coords.push_back(glm::vec3(x, y, 0.0f));
		uv.push_back(glm::vec2(0.0f, 0.0f));
		n.push_back(glm::vec3(0.0F, 0.0F, 1.0F));
		indices.push_back(0);
		indices.push_back(i-1);
		indices.push_back(i);
	}

	indices.push_back(0);
	indices.push_back(precision);
	indices.push_back(1);

	mesh.setVertex(coords, indices, uv, n);
}

void Hand2D::start() {
	Module::start();

	//rectangle(_circle_mesh);
	circle(_circle_mesh, 1.0F, 1000);
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
	}

	if (_modify_vertex) {
		//std::cout << "modify" << std::endl;
		std::vector<glm::vec3>& c = const_cast<std::vector<glm::vec3>&>(_circle_mesh.getCoords());
		std::vector<glm::vec3> cc = c;

		/*glm::vec3 d = c[3] - c[1];
		glm::vec3 normal = glm::normalize(glm::vec3(d.y, -d.x, d.z));

		c[2] = glm::vec3(c[2].x + normal.x, c[2].y + normal.y, c[2].z);*/

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

			pix_x += normal.x * _pix_to_world;
			pix_y += normal.y * _pix_to_world;

			if (pix_x < 0 || pix_x >= _hand_img.getWidth() || pix_y < 0 || pix_y >= _hand_img.getHeight()) {
				continue;
			}

			glm::vec4 pix = _hand_img.getPixel(pix_y, pix_x);
			if (pix.a == 0) {
				continue;
			}

			c[i] = glm::vec3(cc[i].x + normal.x * _pix_to_world, cc[i].y + normal.y * _pix_to_world, cc[i].z);
		}

		DefaultConf::coord_buffer->setValues(
			&Object3D::updateCoords(DefaultConf::renderer, DefaultConf::graphic_shader)[0]
		);

		//_modify_vertex = !_modify_vertex;
	}
}

void Hand2D::close() {
	Module::close();

}