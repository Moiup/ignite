#include "PushConstantInfo.h"

PushConstantInfo::PushConstantInfo() {
	;
}

void PushConstantInfo::setPushConstantRange(VkPushConstantRange& constant_range) {
	_constant_range = constant_range;
}

void PushConstantInfo::setStageFlags(VkShaderStageFlags stage_flags) {
	_constant_range.stageFlags = stage_flags;
}

void PushConstantInfo::setOffset(uint32_t offset) {
	_constant_range.offset = offset;
}

void PushConstantInfo::setSize(uint32_t size) {
	_constant_range.size = size;
}


const VkPushConstantRange& PushConstantInfo::getPushConstantRange() const {
	return _constant_range;
}

VkShaderStageFlags PushConstantInfo::getStageFlags() {
	return _constant_range.stageFlags;
}

uint32_t PushConstantInfo::getOffset() {
	return _constant_range.offset;
}

uint32_t PushConstantInfo::getSize() {
	return _constant_range.size;
}