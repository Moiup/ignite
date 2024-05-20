#pragma once

#include "Module.h"
#include "DispatcherSync.h"

class ReadWriteImageMod : public Module
{

private:
	ComputeShader _comp_read_write_shader{};

	Texture _src_texture{};
	Texture _dst_texture{};

	ComputePipeline _compute_pipeline{};
	DispatcherSync _dispatcher_sync{};

public:
	ReadWriteImageMod();

	void init();
	void start();
	void update();
	void close();

};
