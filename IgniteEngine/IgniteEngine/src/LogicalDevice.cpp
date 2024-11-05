#include "LogicalDevice.h"


LogicalDevice::LogicalDevice()
{
	;
}

bool LogicalDevice::defineQueue(std::string name, std::vector<VkQueueFlagBits> flags, uint32_t count){
	uint32_t family_index{0};
	bool is_flag{false};

	if (_queues.count(name)) {
		return false;
	}

	auto& family_properties = _device.getFamilyProperties();
	for (uint32_t i = 0; i < family_properties.size(); i++) {
		is_flag = isFlag(family_properties[i].queueFamilyProperties.queueFlags, flags);
		if (is_flag) {
			family_index = i;
			break;
		}
	}

	if (!is_flag) {
		return false;
	}


	for (uint32_t q = 0; q < count; q++) {
		//Queue queue{};
		//queue.setDevice(&_device);
		_queues[name].resize(_queues[name].size() + 1);
		_queues[name][_queues[name].size() - 1].setFamilyIndex(family_index);

		//_queues[name].push_back(queue);
	}

	return true;
}

void LogicalDevice::setGPU(PhysicalDevice* gpu) {
	_gpu = gpu;
}

void LogicalDevice::configure() {
	_device.configure(_gpu);
}

std::vector<Queue>& LogicalDevice::getQueues(const std::string& name) {
	return _queues[name];
}

void LogicalDevice::create() {
	std::vector<VkDeviceQueueCreateInfo> queues_info{};
	std::unordered_map<uint32_t, std::vector<float>> priorities{};
	createQueueCreateInfo(queues_info, priorities);
	//_device.configure(_gpu);
	_device.create(queues_info);
	gettingTheQueues();
}

Device* LogicalDevice::getDevice() {
	return &_device;
}

PhysicalDevice* LogicalDevice::getGPU() {
	return _gpu;
}

void LogicalDevice::waitIdle() {
	vkDeviceWaitIdle(_device.getDevice());
}

bool LogicalDevice::isFlag(VkQueueFlags family_flags, std::vector<VkQueueFlagBits> flags) {
	uint32_t i = 0;
	uint32_t n = flags.size();
	bool is_ok = true;

	do {
		is_ok = is_ok && (family_flags & flags[i]);
		i++;
	} while (is_ok && i < n);
	
	return is_ok;
}

void LogicalDevice::createQueueCreateInfo(
	std::vector<VkDeviceQueueCreateInfo>& queues_info,
	std::unordered_map<uint32_t, std::vector<float>>& priorities
	) {

	std::unordered_map<uint32_t, uint32_t> is_fam{};

	for (auto& q : _queues) {
		std::vector<Queue>& queues = q.second;
		const uint32_t fam_i = queues[0].getFamilyIndex();
		if(is_fam.count(fam_i)){
			is_fam[fam_i] += queues.size();
		}
		else {
			is_fam[fam_i] = 0;
		}
	}

	for (auto& fam: is_fam) {
		uint32_t fam_i = fam.first;
		uint32_t count = fam.second;

		priorities[fam_i] = std::vector<float>(count, 0.0f);

		VkDeviceQueueCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.queueFamilyIndex = fam_i;
		info.queueCount = priorities[fam_i].size();
		info.pQueuePriorities = priorities[fam_i].data();

		queues_info.push_back(info);
	}
}

void LogicalDevice::gettingTheQueues() {
	// Getting the queues
	for (auto& n_q : _queues) {
		for (Queue& q : n_q.second) {
			q.setDevice(&_device);
			q.create();
		}
	}
}