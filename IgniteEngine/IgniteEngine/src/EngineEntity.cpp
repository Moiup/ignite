#include "EngineEntity.h"

std::vector<EngineEntity*> EngineEntity::engine_entities;

EngineEntity::EngineEntity() {

	_nb_shared = new int32_t(1);

	engine_entities.push_back(this);
}

EngineEntity::EngineEntity(const EngineEntity& e) {
	*this = e;
}

EngineEntity::~EngineEntity() {
	close();
}

EngineEntity& EngineEntity::operator=(const EngineEntity& e) {
	close();
	_nb_shared = e._nb_shared;
	*_nb_shared += 1;

	return *this;
}

void EngineEntity::init() { ; }

void EngineEntity::start() { ; }

void EngineEntity::update() { ; }

void EngineEntity::close() {
	*_nb_shared -= 1;
	if (*_nb_shared) {
		return;
	}
	delete _nb_shared;

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

	destroy();
}

void EngineEntity::destroy() {
	;
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

