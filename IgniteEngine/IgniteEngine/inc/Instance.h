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
	VkApplicationInfo _application_info{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = nullptr,
		.pApplicationName = "Ignite",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "IgniteEngine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_4
	};

	int32_t* _shared_count{ nullptr };

public:
	Instance();
	Instance(std::vector<char*>& layers);
	Instance(
		std::vector<char*>& layers,
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
	std::vector<PhysicalDevice> enumeratePhysicalDevices();

	PhysicalDevice getGPU(uint32_t gpu_id = 0);
	PhysicalDevice getDefaultGPU();

	static std::vector<VkLayerProperties> getLayers();
	static void displayLayers();

	static std::vector<VkExtensionProperties> enumerateExtensionProperties();
	static void displayExtensionProperties();

private:
	std::vector<const char*> setExtensions();
	void create(std::vector<char*>& layers, std::vector<const char*>& extensions);
	void destroy();
	void destroyInstance();
};

