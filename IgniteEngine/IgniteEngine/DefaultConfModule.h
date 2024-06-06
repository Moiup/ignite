#pragma once

#include "Module.h"

class DefaultConfModule : public Module
{
	using Module::Module;

	void init();
	void start();
	void update();
	void close();
};

