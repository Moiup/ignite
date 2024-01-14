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
	buildImage();

	_is_finished = true;
}

void RayTracerCPU::buildAssets() {
	Camera* camera = (Camera*)DefaultConf::camera->camera();
	std::pair<std::vector<Triangle>, std::vector<Material>> assets = Triangle::buildTriangles(camera->getTranslate() * camera->getRotate());
	_triangles = assets.first;
	_materials = assets.second;
}

void RayTracerCPU::buildImage() {
	Camera* camera = (Camera*)DefaultConf::camera->camera();
	uint64_t width = DefaultConf::render_window->getWidthInPixel();
	uint64_t height = DefaultConf::render_window->getHeightInPixel();

	_image = Texture(
		width,
		height
	);

	glm::mat4 viewport = camera->getViewport(width, height);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera->getView();
	glm::mat4 projection = camera->getProjection();
	glm::mat4 inv = glm::inverse(viewport * projection * view * model);

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

	//_image.writeFile("ray_traced_image.png");
}

void RayTracerCPU::computePixel(uint64_t x, uint64_t y, glm::mat4 inv) {
	Ray ray = Ray(
		inv * glm::vec4(x + 0.5f, y + 0.5f, 0, 1.0),
		inv * glm::vec4(x + 0.5f, y + 0.5f, 1, 1.0)
	);

	Hit hit;

	// Going through the triangles
	for (Triangle& triangle : _triangles) {
		Hit hit_tmp = triangle.intersect(ray);

		if (hit_tmp < hit) {
			hit = hit_tmp;
		}
	}
	
	glm::vec4 pix{};
	if (hit) {
		pix = glm::vec4(255, 0, 0, 255);
	}
	else {
		pix = glm::vec4(0, 0, 0, 255);
	}

	_image.setPixel(pix, y, x);
}

void RayTracerCPU::computePixel(float x, float y, uint64_t p_x, uint64_t p_y) {
	Ray ray = Ray(
		glm::vec3(0, 0, 0),
		glm::vec3(x, y, 1.0)
	);

	Hit hit;

	// Going through the triangles
	for (Triangle& triangle : _triangles) {
		Hit hit_tmp = triangle.intersectSimple(ray);

		if (hit_tmp < hit) {
			hit = hit_tmp;
		}
	}

	glm::vec4 pix{};
	if (hit) {
		pix = glm::vec4(255, 0, 0, 255);
	}
	else {
		pix = glm::vec4(0, 0, 0, 255);
	}

	_image.setPixel(pix, p_y, p_x);
}