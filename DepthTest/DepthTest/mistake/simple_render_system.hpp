#pragma once

#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
	class SimpleRenderSystem {
	LveDevice& lveDevice;

	std::unique_ptr<LvePipeline> lvePipeline;
	VkPipelineLayout             pipelineLayout;

	void createPipelineLayout();
	void createPipeline(VkRenderPass renderPass);

public:
	SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass);
	~SimpleRenderSystem();

	void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects);

	SimpleRenderSystem(const SimpleRenderSystem&)            = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
};

} // namespace lve
