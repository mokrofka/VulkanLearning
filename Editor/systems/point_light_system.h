#pragma once

#include "LveCamera.h"
#include "LveDevice.h"
#include "LveGameObject.h"
#include "LvePipeline.h"
// std

#include "LveFrameInfo.h"

#include <memory>

class PointLightSystem {
	LveDevice& Device;

	std::unique_ptr<LvePipeline> Pipeline;
	VkPipelineLayout             pipelineLayout;

	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

public:
	PointLightSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~PointLightSystem();

	void update(FrameInfo& frameInfo, GlobalUbo& ubo);
	void render(FrameInfo& frameInfo);

	PointLightSystem(const PointLightSystem&)            = delete;
	PointLightSystem& operator=(const PointLightSystem&) = delete;
};
