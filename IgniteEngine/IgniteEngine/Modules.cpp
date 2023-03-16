#include "Modules.h"

Modules::Modules() :
	_graphics_shader{ nullptr },
	_renderer{ nullptr },
	_camera{}
{
	;
}

void Modules::init() {
	for (auto& mod : Module::modules) {
		mod->setGraphicsShader(_graphics_shader);
		mod->setRenderer(_renderer);
		mod->setCamera(_camera);
	}
}

void Modules::setGraphicsShader(GraphicShader* graphics_shader) {
	_graphics_shader = graphics_shader;
}

void Modules::setRenderer(Renderer* renderer) {
	_renderer = renderer;
}

void Modules::setCamera(Camera* camera) {
	_camera = camera;
}