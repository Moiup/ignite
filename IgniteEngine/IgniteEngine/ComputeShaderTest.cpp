#include "ComputeShaderTest.h"

ComputeShaderTest::ComputeShaderTest() {
	;
}

void ComputeShaderTest::init() {
	Module::init();


}

void ComputeShaderTest::start() {
	Module::start();

	_compute_shader.setLogicalDevice(DefaultConf::logical_device);
	_compute_shader.read("../shaders/compute_sum.comp");

	_compute_shader.addStorageBufferInfo(
		"inputData",
		0,
		VK_SHADER_STAGE_COMPUTE_BIT
	);
	_compute_shader.addStorageBufferInfo(
		"outputData",
		1,
		VK_SHADER_STAGE_COMPUTE_BIT
	);

	std::vector<int> input_data_vec(10, 0);

	_input_data.setQueue(DefaultConf::compute_queue);
	_input_data.setSize(input_data_vec.size() * sizeof(input_data_vec[0]));
	_input_data.create();
	_input_data.setValues(input_data_vec.data());
	_compute_shader.addStorageBuffer("inputData", &_input_data);


	_output_data.setQueue(DefaultConf::compute_queue);
	_output_data.setSize(input_data_vec.size() * sizeof(input_data_vec[0]));
	_output_data.create();
	_compute_shader.addStorageBuffer("outputData", &_output_data);

	// The compute pipeline
	_compute_pipeline.setShader(&_compute_shader);
	_compute_pipeline.create();

	// The dispatcher
	_dispatcher_sync.setQueue(&DefaultConf::logical_device->getQueues("compute_queues")[0]);
	_dispatcher_sync.setComputePipeline(&_compute_pipeline);
	_dispatcher_sync.create();
}

void ComputeShaderTest::update() {
	Module::update();

	std::cout << "DISPATCH" << std::endl;
	_dispatcher_sync.dispatch(10, 1, 1);

	Pointer<uint8_t> tmp = _output_data.getValues();
	int* output = reinterpret_cast<int*>(&tmp[0]);
	for (uint32_t i = 0; i < 10; ++i) {
		std::cout << output[i] << std::endl;
	}
}

void ComputeShaderTest::close() {
	Module::close();

	_input_data.destroy();
	_output_data.destroy();

	_compute_pipeline.destroy();
	_dispatcher_sync.destroy();
	_compute_shader.destroy();
}
