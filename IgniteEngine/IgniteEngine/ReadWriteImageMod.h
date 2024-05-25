#pragma once

#include "Module.h"
#include "DispatcherSync.h"

struct CRWSPushConstant {
	uint32_t width;
	uint32_t height;
	uint32_t color_sum;
};

class ReadWriteImageMod : public Module
{

private:
	ComputeShader _comp_read_write_shader{};

	CRWSPushConstant _crws_pc{};

	LoadedObjectInfo _plan_info{};
	Object3D _plan;

	StorageBuffer _input_data{};

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
