#include "Instance.h"

Instance::Instance() {
	setApplicationInfo(
		"Ingnite",
		VK_MAKE_VERSION(1, 0, 0),
		"IngiteEngine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_3
	);
}

Instance::Instance(
	std::string application_name,
	uint32_t application_version,
	std::string engine_name,
	uint32_t engine_version,
	uint32_t api_version
) {
	setApplicationInfo(
		application_name,
		application_version,
		engine_name,
		engine_version,
		api_version
	);
}

void Instance::setApplicationInfo(
	std::string application_name,
	uint32_t application_version,
	std::string engine_name,
	uint32_t engine_version,
	uint32_t api_version
) {
	_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	_application_info.pNext = nullptr;
	_application_info.pApplicationName = application_name.c_str();
	_application_info.applicationVersion = application_version;
	_application_info.pEngineName = engine_name.c_str();
	_application_info.engineVersion = engine_version;
	_application_info.apiVersion = api_version;
}

void Instance::setExtensionsAndLayers(std::vector<std::string> layer_arr) {
	// Instance Extensions
	/*_extensions = glfwGetRequiredInstanceExtensions(&_extension_count);*/

	if (SDL_Vulkan_GetInstanceExtensions(nullptr, &_extension_count, nullptr) != SDL_TRUE) {
		throw std::runtime_error("No extension found!");
	}

	_extensions.resize(_extension_count);
	bool test = SDL_Vulkan_GetInstanceExtensions(nullptr, &_extension_count, _extensions.data());

	std::cout << _extension_count << " supported extensions:" << std::endl;
	for (uint32_t i = 0; i < _extension_count; i++){
		std::cout << "\t" << _extensions[i] << std::endl;
	}

	// Instance Layers
	uint32_t instance_layer_prop_count = 0;
	std::vector<VkLayerProperties> layer_properties{};
	vkEnumerateInstanceLayerProperties(&instance_layer_prop_count, nullptr);
	layer_properties.resize(instance_layer_prop_count);
	vkEnumerateInstanceLayerProperties(&instance_layer_prop_count, layer_properties.data());

	std::cout << "Available layers:" << std::endl;
	for (std::string layer : layer_arr) {
		for (const auto& layer_prop: layer_properties) {
			std::string lp = std::string(layer_prop.layerName);
			if (layer == lp) {
				_available_layers.push_back(_strdup(layer_prop.layerName));
				std::cout << "\t" << lp << std::endl;
			}
		}
	}
}

const VkInstance& Instance::getInstance() const {
	return _instance;
}

void Instance::create() {
	VkInstanceCreateInfo vk_instance_create_info{};
	vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instance_create_info.pNext = nullptr;
	vk_instance_create_info.flags = 0;
	vk_instance_create_info.pApplicationInfo = &_application_info;
	vk_instance_create_info.enabledLayerCount = _available_layers.size();
	vk_instance_create_info.ppEnabledLayerNames = _available_layers.data();
	vk_instance_create_info.enabledExtensionCount = _extension_count;
	vk_instance_create_info.ppEnabledExtensionNames = _extensions.data();

	VkResult vk_result = vkCreateInstance(&vk_instance_create_info, nullptr, &_instance);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Failed!" << std::endl;
		throw std::runtime_error("Error: Instance creation failed!");
	}

	std::cout << "Instance created." << std::endl;
}

void Instance::destroy() {
	if (_instance) {
		vkDestroyInstance(_instance, nullptr);

		for (auto layer : _available_layers) {
			free(layer);
		}
	}
}