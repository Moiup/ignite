#include "Module.h"

std::vector<Module*> Module::modules;

Module::Module() :
	EngineEntity::EngineEntity(),
	_graphics_shader{},
	_renderer{}
{
	;
}

void Module::setGraphicsShader(GraphicShader* graphics_shader) {
	_graphics_shader = graphics_shader;
}

void Module::setRenderer(Renderer* renderer) {
	_renderer = renderer;
}

void Module::init() {
	EngineEntity::init();
	modules.push_back(this);
}

void Module::start() {
	EngineEntity::start();

}

void Module::update() {
	EngineEntity::update();

}

void Module::close() {
	EngineEntity::close();

	uint32_t i = 0;
	for (Module* mod : Module::modules) {
		if (mod == this) {
			Module::modules.erase(
				Module::modules.begin() + i
			);
			break;
		}
		i++;
	}
}