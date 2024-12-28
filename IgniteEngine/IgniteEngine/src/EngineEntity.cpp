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
	EngineEntity::destroy();
	deleteEntity(*this);
}

EngineEntity& EngineEntity::operator=(const EngineEntity& e) {
	EngineEntity::destroy();

	_nb_shared = e._nb_shared;
	*_nb_shared += 1;

	return *this;
}

int32_t EngineEntity::getNbShared() {
	return *_nb_shared;
}

void EngineEntity::init() { ; }

void EngineEntity::start() { ; }

void EngineEntity::update() { ; }

void EngineEntity::close() {
	//destroy();
}

void EngineEntity::destroy() {
	if (!_nb_shared) {
		return;
	}

	*_nb_shared -= 1;
	if (*_nb_shared) {
		return;
	}
	delete _nb_shared;
}

void EngineEntity::clean() {
	EngineEntity::destroy();
}

void EngineEntity::deleteEntity(const EngineEntity& entity) {
	uint32_t i = 0;
	for (EngineEntity* e : EngineEntity::engine_entities) {
		if (e == &entity) {
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

