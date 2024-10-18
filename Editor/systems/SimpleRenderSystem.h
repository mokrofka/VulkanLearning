#pragma once

#include "LveCamera.h"
#include "LveGameObject.h"
#include "LveDevice.h"
#include "LvePipeline.h"
// std

#include "LveFrameInfo.h"

#include <memory>

class SimpleRenderSystem {
	LveDevice& Device;

	std::unique_ptr<LvePipeline> Pipeline;
	VkPipelineLayout             pipelineLayout;

	void createPipelineLayout(std::vector<VkDescriptorSetLayout> globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

public:
	SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& globalSetLayout);
	~SimpleRenderSystem();

	void renderGameObjects(FrameInfo& frameInfo);

	SimpleRenderSystem(const SimpleRenderSystem&)            = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
};
