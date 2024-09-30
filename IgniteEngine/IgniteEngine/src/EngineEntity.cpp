#include "EngineEntity.h"

std::vector<EngineEntity*> EngineEntity::engine_entities;

EngineEntity::EngineEntity() {
	engine_entities.push_back(this);
}

void EngineEntity::init() { ; }

void EngineEntity::start() { ; }

void EngineEntity::update() { ; }

void EngineEntity::close() {
	uint32_t i = 0;
	for (EngineEntity* e : EngineEntity::engine_entities) {
		if (e == this) {
			EngineEntity::engine_entities.erase(
				EngineEntity::engine_entities.begin() + i
			);
			break;
		}
		i++;
	}
}

void EngineEntity::initAll() {
	for (EngineEntity* ee : EngineEntity::engine_entities) {
		ee->init();
	}
}

void EngineEntity::startAll() {
	for (EngineEntity* ee : EngineEntity::engine_entities) {
		ee->start();
	}
}

void EngineEntity::updateAll() {
	for (EngineEntity* ee : EngineEntity::engine_entities) {
		ee->update();
	}
}

void EngineEntity::closeAll() {
	for (EngineEntity* ee : EngineEntity::engine_entities) {
		ee->close();
	}
}

