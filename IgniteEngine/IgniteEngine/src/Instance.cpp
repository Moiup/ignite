#include "Instance.h"

Instance::Instance() {
	setApplicationInfo(
		"Ignite",
		VK_MAKE_VERSION(1, 0, 0),
		"IngiteEngine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_3
	);

	_shared_count = new int32_t(1);
}

Instance::Instance(std::vector<char*>& layers) :
	Instance::Instance()
{
	std::vector<const char*> extensions = setExtensions();
	create(layers, extensions);
}

Instance::Instance(
	std::vector<char*>& layers,
	std::string application_name,
	uint32_t application_version,
	std::string engine_name,
	uint32_t engine_version,
	uint32_t api_version
) :
	Instance::Instance()
{
	std::vector<const char*> extensions = setExtensions();
	setApplicationInfo(
		application_name,
		application_version,
		engine_name,
		engine_version,
		api_version
	);
	create(layers, extensions);
}

Instance::Instance(const Instance& instance) {
	*this = instance;
}

Instance::Instance(Instance&& instance) {
	*this = std::move(instance);
}

Instance& Instance::operator=(const Instance& instance) {
	destroy();
	_instance = instance._instance;
	_application_info = instance._application_info;

	_shared_count = instance._shared_count;
	*_shared_count += 1;

	return *this;
}

Instance& Instance::operator=(Instance&& instance) {
	destroy();
	_instance = std::move(instance)._instance;
	instance._instance = nullptr;
	_application_info = std::move(instance)._application_info;

	_shared_count = std::move(instance)._shared_count;
	instance._shared_count = nullptr;

	return *this;
}

Instance::~Instance() {
	destroy();
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

std::vector<const char*> Instance::setExtensions() {
	// Instance Extensions
	/*_extensions = glfwGetRequiredInstanceExtensions(&_extension_count);*/

	uint32_t extension_count{ 0 };
	if (SDL_Vulkan_GetInstanceExtensions(nullptr, &extension_count, nullptr) != SDL_TRUE) {
		throw std::runtime_error("No extension found!");
	}

	std::vector<const char*> extensions(extension_count);
	bool test = SDL_Vulkan_GetInstanceExtensions(nullptr, &extension_count, extensions.data());
	
	std::cout << extension_count << " supported extensions:" << std::endl;
	for (uint32_t i = 0; i < extension_count; i++){
		std::cout << "\t" << extensions[i] << std::endl;
	}

	return extensions;
}

std::vector<VkLayerProperties> Instance::getLayers() {
	// Instance Layers
	uint32_t instance_layer_prop_count = 0;
	std::vector<VkLayerProperties> layer_properties{};
	vkEnumerateInstanceLayerProperties(&instance_layer_prop_count, nullptr);
	layer_properties.resize(instance_layer_prop_count);
	vkEnumerateInstanceLayerProperties(&instance_layer_prop_count, layer_properties.data());

	return layer_properties;
}

void Instance::displayLayers() {
	std::vector<VkLayerProperties> layer_prop = Instance::getLayers();

	std::cout << "Available layers:" << std::endl;
	for (auto& prop : layer_prop) {
		std::cout << "    " << prop.layerName << std::endl;
	}
}

const VkInstance& Instance::getInstance() const {
	return _instance;
}

void Instance::displayAvailableGPUs() {
	std::vector<PhysicalDevice> gpus = enumeratePhysicalDevices();

	int32_t gpu_id = 0;
	std::cout << "--------------------------" << std::endl;
	for (auto& gpu : gpus) {
		std::cout << "gpu id: " << gpu_id << std::endl;
		gpu.displayProperties();
		std::cout << "--------------------------" << std::endl;

		gpu_id++;
	}
}

PhysicalDevice Instance::getGPU(uint32_t gpu_id) {
	std::vector<PhysicalDevice> gpus = enumeratePhysicalDevices();
	return gpus[gpu_id];
}

PhysicalDevice Instance::getDefaultGPU() {
	std::vector<PhysicalDevice> gpus = enumeratePhysicalDevices();
	const VkPhysicalDeviceType gpu_type[] = {
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
		VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU,
		VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU,
		VK_PHYSICAL_DEVICE_TYPE_CPU,
		VK_PHYSICAL_DEVICE_TYPE_OTHER
	};
	int32_t nb_gpu_type = sizeof(gpu_type) / sizeof(VkPhysicalDeviceType);

	std::unordered_map<VkPhysicalDeviceType, PhysicalDevice*> type_to_gpu;

	for (auto& gpu : gpus) {
		type_to_gpu[gpu.getProperties().deviceType] = &gpu;
	}
	
	for (int32_t i = 0; i < nb_gpu_type; ++i) {
		if (type_to_gpu.count(gpu_type[i])) {
			return *type_to_gpu[gpu_type[i]];
		}
	}

	return gpus[0];
}

std::vector<PhysicalDevice> Instance::enumeratePhysicalDevices() {
	std::vector<VkPhysicalDevice> vk_gpus{};
	std::vector<PhysicalDevice> gpus{};
	uint32_t gpus_count{ 0 };
	VkResult vk_result{};

	vk_result = vkEnumeratePhysicalDevices(
		_instance,
		&gpus_count,
		nullptr
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Unable to get physical device count.");
	}
	vk_gpus.resize(gpus_count);
	vk_result = vkEnumeratePhysicalDevices(
		_instance,
		&gpus_count,
		vk_gpus.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Unable to get the physical device(s).");
	}

	for (auto& vk_gpu : vk_gpus) {
		gpus.push_back(PhysicalDevice(vk_gpu));
	}

	return gpus;
}

std::vector<VkExtensionProperties> Instance::enumerateExtensionProperties() {
	std::vector<VkExtensionProperties> extensions{};
	uint32_t nb_ext{ 0 };
	
	vkEnumerateInstanceExtensionProperties(
		nullptr,
		&nb_ext,
		nullptr
	);

	extensions.resize(nb_ext);
	vkEnumerateInstanceExtensionProperties(
		nullptr,
		&nb_ext,
		extensions.data()
	);

	return extensions;
}

void Instance::displayExtensionProperties() {
	std::vector<VkExtensionProperties> extensions = Instance::enumerateExtensionProperties();

	std::cout << "Instance extension properties (" << extensions.size() << "): " << std::endl;
	for (auto& ext : extensions) {
		std::cout << "    " << ext.extensionName << ": " << ext.specVersion << std::endl;
	}
}

void Instance::create(std::vector<char*>& layers, std::vector<const char*>& extensions) {
	VkInstanceCreateInfo vk_instance_create_info{};
	vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instance_create_info.pNext = nullptr;
	vk_instance_create_info.flags = 0;
	vk_instance_create_info.pApplicationInfo = &_application_info;
	vk_instance_create_info.enabledLayerCount = layers.size();
	vk_instance_create_info.ppEnabledLayerNames = layers.data();
	vk_instance_create_info.enabledExtensionCount = extensions.size();
	vk_instance_create_info.ppEnabledExtensionNames = extensions.data();

	VkResult vk_result = vkCreateInstance(&vk_instance_create_info, nullptr, &_instance);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Failed!" << std::endl;
		throw std::runtime_error("Error: Instance creation failed!");
	}

	std::cout << "Instance created." << std::endl;
}

void Instance::destroy() {
	if (!_shared_count) {
		return;
	}

	*_shared_count -= 1;
	if (*_shared_count) {
		return;
	}
	delete _shared_count;
	_shared_count = nullptr;

	destroyInstance();
}

void Instance::destroyInstance() {
	if (!_instance) {
		return;
	}
	vkDestroyInstance(_instance, nullptr);
	_instance = nullptr;
}
