#pragma once
#include "LveDevice.h"

#include <vulkan/vulkan_core.h>
#include <memory>
#include <unordered_map>

class LveDescriptorSetLayout {
	LveDevice&            lveDevice;
	VkDescriptorSetLayout descriptorSetLayout;
	std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings;

	friend class LveDescriptorWriter;

public:
	class Builder {
		LveDevice& lveDevice;
		std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings{};

	public:
		Builder(LveDevice& lveDevice) : lveDevice{lveDevice} {}
		
		Builder& addBinding(
			u32                binding,
			VkDescriptorType   descriptorType,
			VkShaderStageFlags stageFlags,
			u32                count = 1);
		std::unique_ptr<LveDescriptorSetLayout> build() const;
	};

	LveDescriptorSetLayout(LveDevice& lveDevice, std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings);
	~LveDescriptorSetLayout();
	LveDescriptorSetLayout(const LveDescriptorSetLayout&)            = delete;
	LveDescriptorSetLayout& operator=(const LveDescriptorSetLayout&) = delete;

	VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
};

class LveDescriptorPool {
	LveDevice&       lveDevice;
	VkDescriptorPool descriptorPool;

	friend class LveDescriptorWriter;

public:
	class Builder {
		LveDevice&                        lveDevice;
		std::vector<VkDescriptorPoolSize> poolSizes{};
		u32                               maxSets   = 1000;
		VkDescriptorPoolCreateFlags       poolFlags = 0;

	public:
		Builder(LveDevice& lveDevice) : lveDevice{lveDevice} {}

		Builder& addPoolSize(VkDescriptorType descriptorType, u32 count);
		Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder& setMaxSets(u32 count);
		std::unique_ptr<LveDescriptorPool> build() const;
	};

	LveDescriptorPool(
		LveDevice&                               lveDevice,
		u32                                      maxSets,
		VkDescriptorPoolCreateFlags              poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes);
	~LveDescriptorPool();
	LveDescriptorPool(const LveDescriptorPool&)            = delete;
	LveDescriptorPool& operator=(const LveDescriptorPool&) = delete;

	b8 allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

	void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

	void resetPool();
};

class LveDescriptorWriter {
	LveDescriptorSetLayout&           setLayout;
	LveDescriptorPool&                pool;
	std::vector<VkWriteDescriptorSet> writes;

public:
	LveDescriptorWriter(LveDescriptorSetLayout& setLayout, LveDescriptorPool& pool);

	LveDescriptorWriter& writeBuffer(u32 binding, VkDescriptorBufferInfo* bufferInfo);
	LveDescriptorWriter& writeImage(u32 binding, VkDescriptorImageInfo* imageInfo);

	b8 build(VkDescriptorSet& set);
	void overwrite(VkDescriptorSet& set);
};
