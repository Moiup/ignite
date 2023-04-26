#include "Module.h"

std::vector<Module*> Module::modules;

Module::Module() :
	EngineEntity::EngineEntity()
{
	;
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