#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <SDL_vulkan.h>

#include <PhysicalDevice.h>

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

class Instance
{
private:
	VkInstance _instance{ nullptr };
	VkApplicationInfo _application_info;
	std::vector<char*> _available_layers;
	uint32_t _extension_count{};
	//const char** _extensions;
	std::vector<const char*> _extensions;

	int32_t* _shared_count{ nullptr };

public:
	Instance();
	Instance(std::vector<std::string>& layer_arr);
	Instance(
		std::vector<std::string>& layer_arr,
		std::string application_name,
		uint32_t application_version,
		std::string engine_name,
		uint32_t engine_version,
		uint32_t api_version
	);

	Instance(const Instance& instance);
	Instance(Instance&& instance);

	Instance& operator=(const Instance& instance);
	Instance& operator=(Instance&& instance);

	~Instance();

	void setApplicationInfo(
		std::string application_name,
		uint32_t application_version,
		std::string engine_name,
		uint32_t engine_version,
		uint32_t api_version
	);

	const VkInstance& getInstance() const;

	void displayAvailableGPUs();

	PhysicalDevice getGPU(uint32_t gpu_id = 0);
	PhysicalDevice getDefaultGPU();

	static std::vector<VkLayerProperties> getLayers();
	static void displayLayers();

private:
	void setExtensionsAndLayers(std::vector<std::string>& layer_arr);
	void create();
	void destroy();
	void destroyInstance();
	void freeLayers();
	std::vector<PhysicalDevice> enumeratePhysicalDevices();
};

