#include "LveDescriptor.h"

// std
#include <cassert>
#include <stdexcept>

LveDescriptorSetLayout::Builder& LveDescriptorSetLayout::Builder::addBinding(
	u32                binding,
	VkDescriptorType   descriptorType,
	VkShaderStageFlags stageFlags,
	u32                count) {
	assert(bindings.count(binding) == 0 && "Binding already in use");
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding         = binding;
	layoutBinding.descriptorType  = descriptorType;
	layoutBinding.descriptorCount = count;
	layoutBinding.stageFlags      = stageFlags;
	bindings[binding]             = layoutBinding;
	return *this;
}

std::unique_ptr<LveDescriptorSetLayout> LveDescriptorSetLayout::Builder::build() const {
	return std::make_unique<LveDescriptorSetLayout>(lveDevice, bindings);
}

// *************** Descriptor Set Layout *********************

LveDescriptorSetLayout::LveDescriptorSetLayout(LveDevice& lveDevice, std::unordered_map<u32,
											   VkDescriptorSetLayoutBinding> bindings)
	: lveDevice{lveDevice}, bindings{bindings} {
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};

	for (auto kv : bindings) {
		setLayoutBindings.push_back(kv.second);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = (u32)setLayoutBindings.size();
	descriptorSetLayoutInfo.pBindings    = setLayoutBindings.data();

	if (vkCreateDescriptorSetLayout(lveDevice.device, &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

LveDescriptorSetLayout::~LveDescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(lveDevice.device, descriptorSetLayout, nullptr);
}

// *************** Descriptor Pool Builder *********************

LveDescriptorPool::Builder& LveDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, u32 count) {
	poolSizes.push_back({descriptorType, count});
	return *this;
}

LveDescriptorPool::Builder& LveDescriptorPool::Builder::setPoolFlags(
	VkDescriptorPoolCreateFlags flags) {
	poolFlags = flags;
	return *this;
}

LveDescriptorPool::Builder& LveDescriptorPool::Builder::setMaxSets(u32 count) {
	maxSets = count;
	return *this;
}

std::unique_ptr<LveDescriptorPool> LveDescriptorPool::Builder::build() const {
	return std::make_unique<LveDescriptorPool>(lveDevice, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

LveDescriptorPool::LveDescriptorPool(LveDevice& lveDevice, u32 maxSets, VkDescriptorPoolCreateFlags poolFlags,
                                     const std::vector<VkDescriptorPoolSize>& poolSizes)
	: lveDevice{lveDevice} {
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = (u32)poolSizes.size();
	descriptorPoolInfo.pPoolSizes    = poolSizes.data();
	descriptorPoolInfo.maxSets       = maxSets;
	descriptorPoolInfo.flags         = poolFlags;

	if (vkCreateDescriptorPool(lveDevice.device, &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

LveDescriptorPool::~LveDescriptorPool() {
	vkDestroyDescriptorPool(lveDevice.device, descriptorPool, nullptr);
}

b8 LveDescriptorPool::allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool     = descriptorPool;
	allocInfo.pSetLayouts        = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
	// a new pool whenever an old pool fills up. But this is beyond our current scope
	if (vkAllocateDescriptorSets(lveDevice.device, &allocInfo, &descriptor) != VK_SUCCESS) {
		return false;
	}
	return true;
}

void LveDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
	vkFreeDescriptorSets(
		lveDevice.device,
		descriptorPool,
		(u32)descriptors.size(),
		descriptors.data());
}

void LveDescriptorPool::resetPool() {
	vkResetDescriptorPool(lveDevice.device, descriptorPool, 0);
}

// *************** Descriptor Writer *********************

LveDescriptorWriter::LveDescriptorWriter(LveDescriptorSetLayout& setLayout, LveDescriptorPool& pool)
	: setLayout{setLayout}, pool{pool} {}

LveDescriptorWriter& LveDescriptorWriter::writeBuffer(u32 binding, VkDescriptorBufferInfo* bufferInfo) {
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = setLayout.bindings[binding];

	assert(bindingDescription.descriptorCount == 1 &&
		"Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType  = bindingDescription.descriptorType;
	write.dstBinding      = binding;
	write.pBufferInfo     = bufferInfo;

	write.descriptorCount = 1;

	writes.push_back(write);
	return *this;
}

LveDescriptorWriter& LveDescriptorWriter::writeImage(
	u32 binding, VkDescriptorImageInfo* imageInfo) {
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = setLayout.bindings[binding];

	assert(
		bindingDescription.descriptorCount == 1 &&
		"Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType  = bindingDescription.descriptorType;
	write.dstBinding      = binding;
	write.pImageInfo      = imageInfo;
	write.descriptorCount = 1;

	writes.push_back(write);
	return *this;
}

b8 LveDescriptorWriter::build(VkDescriptorSet& set) {
	b8 success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
	if (!success) {
		return false;
	}
	overwrite(set);
	return true;
}

void LveDescriptorWriter::overwrite(VkDescriptorSet& set) {
	for (auto& write : writes) {
		write.dstSet = set;
	}
	vkUpdateDescriptorSets(pool.lveDevice.device, (u32)writes.size(), writes.data(), 0, nullptr);
}
