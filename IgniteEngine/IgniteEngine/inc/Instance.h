#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <vector>
#include <string>

class Instance
{
private:
	VkInstance _instance{ nullptr };
	VkApplicationInfo _application_info;
	std::vector<char*> _available_layers;
	uint32_t _extension_count{};
	//const char** _extensions;
	std::vector<const char*> _extensions;

public:
	Instance();
	Instance(
		std::string application_name,
		uint32_t application_version,
		std::string engine_name,
		uint32_t engine_version,
		uint32_t api_version
	);

	void setApplicationInfo(
		std::string application_name,
		uint32_t application_version,
		std::string engine_name,
		uint32_t engine_version,
		uint32_t api_version
	);

	void setExtensionsAndLayers(std::vector<std::string> layer_arr);

	const VkInstance& getInstance() const;

	void create();
	void destroy();
};

