#include "RayTracerCPU.h"

RayTracerCPU::RayTracerCPU() :
	_nb_cores{1},
	_is_finished{ true }
{
	;
}

int RayTracerCPU::getNbCores() {
	return _nb_cores;
}

void RayTracerCPU::setNbCores(int nb_cores) {
	_nb_cores = nb_cores;
}

bool RayTracerCPU::getIsFinished() {
	return _is_finished;
}

void RayTracerCPU::setIsFinished(bool is_finished) {
	_is_finished = is_finished;
}

void RayTracerCPU::render() {
	buildAssets();

	std::cout << _scene.string() << std::endl;

	buildImage();

	_is_finished = true;
}

void RayTracerCPU::buildAssets() {
	Camera* camera = (Camera*)DefaultConf::camera->camera();

	_scene.clear();	
	Triangle::buildTriangles(_scene);
}

void RayTracerCPU::buildImage() {
	Camera* camera = const_cast<Camera*>(DefaultConf::camera->camera());
	uint64_t width = DefaultConf::render_window->getWidthInPixel();
	uint64_t height = DefaultConf::render_window->getHeightInPixel();

	_image.setPixels(
		width,
		height
	);


	glm::mat4 model = camera->getTranslate() * camera->getRotate() * camera->getScale();
	glm::mat4 view = camera->getView();

	glm::mat4 projection = camera->getProjection();
	glm::mat4 viewport = camera->getViewport(width, height);
	glm::mat4 clip = camera->getClip();

	glm::mat4 mvpv = viewport * clip * projection * view * model;
	//glm::mat4 mvpv = viewport * clip;
	
	glm::mat4 inv = glm::inverse(mvpv);


	//std::cout << "model" << std::endl;
	//for (uint32_t i = 0; i < 4; i++) {
	//	for (uint32_t j = 0; j < 4; j++) {
	//		std::cout << model[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//std::cout << std::endl;

	std::cout << "view" << std::endl;
	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 4; j++) {
			std::cout << view[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "projection" << std::endl;
	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 4; j++) {
			std::cout << projection[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "viewport" << std::endl;
	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 4; j++) {
			std::cout << viewport[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "mvpv" << std::endl;
	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 4; j++) {
			std::cout << mvpv[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "inv" << std::endl;
	for (uint32_t i = 0; i < 4; i++) {
		for (uint32_t j = 0; j < 4; j++) {
			std::cout << inv[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;



	for (uint64_t y = 0; y < height; y++) {
		for (uint64_t x = 0; x < width; x++) {

			computePixel(x, y, inv);
			//computePixel(
			//	float(x) / float(width) * 2 - 1,
			//	float(y) / float(height) * 2 - 1,
			//	x,
			//	y
			//);
		}
	}

	_image.saveFile("../../assets/ray_traced_image.png");
}

void RayTracerCPU::computePixel(uint64_t x, uint64_t y, glm::mat4 inv) {
	glm::vec4 o = inv * glm::vec4(
		static_cast<float>(x) + 0.5f,
		static_cast<float>(y) + 0.5f,
		0,
		1.0
	);
	glm::vec4 e = inv * glm::vec4(
		static_cast<float>(x) + 0.5f,
		static_cast<float>(y) + 0.5f,
		1.0,
		1.0
	);
	o /= o.w;
	e /= e.w;

	Ray ray = Ray(
		o,
		e
	);

	//std::cout << "x:" << x << " y:" << y << std::endl;
	//std::cout << o.x << " " << o.y << " " << o.z << " " << o.w << std::endl;
	//std::cout << e.x << " " << e.y << " " << e.z << " " << e.w << std::endl;
	//std::cout << std::endl;

	Hit hit(1.0);
	Triangle tr;
	// Going through the triangles
	for (Triangle& triangle : _scene._triangles) {
		Hit hit_tmp = triangle.intersect(ray);

		if (hit_tmp < hit) {
			hit = hit_tmp;
			tr = triangle;
		}
	}
	
	glm::vec4 pix{};
	if (hit.t() < 1.0f) {
		pix = glm::vec4(_scene._materials[tr.mat_id()].Kd, 1.0);
		pix = pix * 255.0f;
	}
	else {
		pix = glm::vec4(0, 0, 0, 255);
	}

	_image.setPixel(y, x, pix);
}

void RayTracerCPU::computePixel(float x, float y, uint64_t p_x, uint64_t p_y) {
	Ray ray = Ray(
		glm::vec3(0, 0, 0),
		glm::vec3(x, y, 1.0)
	);

	Hit hit;
	Triangle tri;

	// Going through the triangles
	for (Triangle& triangle : _scene._triangles) {
		Hit hit_tmp = triangle.intersectSimple(ray);

		if (hit_tmp < hit) {
			hit = hit_tmp;
			tri = triangle;
		}
	}
	
	//if (hit) {
	//	std::cout << std::to_string(tri.A().x) << " __ " << std::to_string(tri.A().y) << " __ " << std::to_string(tri.A().z) << std::endl;

	//	std::cout << std::to_string(tri.B().x) << " __ " << std::to_string(tri.B().y) << " __ " << std::to_string(tri.B().z) << std::endl;

	//	std::cout << std::to_string(tri.C().x) << " __ " << std::to_string(tri.C().y) << " __ " << std::to_string(tri.C().z) << std::endl;
	//	exit(0);
	//}

	glm::vec4 pix{};
	if (hit) {
		pix = glm::vec4(_scene._materials[tri.mat_id()].Kd, 1.0);
		pix = pix * 255.0f;
	}
	else {
		pix = glm::vec4(0, 0, 0, 255);
	}

	_image.setPixel(p_y, p_x, pix);
}