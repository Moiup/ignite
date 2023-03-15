#include "Modules.h"

Modules::Modules() :
	_graphics_shader{ nullptr },
	_renderer{ nullptr }
{
	;
}

void Modules::init() {
	for (auto& mod : Module::modules) {
		mod->setGraphicsShader(_graphics_shader);
		mod->setRenderer(_renderer);
	}
}

void Modules::setGraphicsShader(GraphicShader* graphics_shader) {
	_graphics_shader = graphics_shader;
}

void Modules::setRenderer(Renderer* renderer) {
	_renderer = renderer;
}