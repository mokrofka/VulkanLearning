#include "simple_render_system.hpp"

// libs
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ONE


struct SimplePushCOnstantData {
	glm::mat4 transform{1.0f};
	alignas(16)glm::vec3 color;
};

SimpleRenderSystem::SimpleRenderSystem(LveDevice& lveDevice, VkRenderPass renderPass) :
	lveDevice(lveDevice) {
	createPipelineLayout();
	createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
	vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout() {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset     = 0;
	pushConstantRange.size       = sizeof(SimplePushCOnstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount         = 0;
	pipelineLayoutInfo.pSetLayouts            = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
	if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before swapchain");

	PipelineConfigInfo pipelineConfig{};
	LvePipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	lvePipeline = std::make_unique<LvePipeline>(lveDevice, "simple_shader.vert", "simple_shader.frag", pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject>& gameObjects) {
	lvePipeline->bind(commandBuffer);

	for (LveGameObject& obj : gameObjects) {
		obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<f32>());
		obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.01f, glm::two_pi<f32>());

		SimplePushCOnstantData push{};
		push.color     = obj.color;
		push.transform = obj.transform.mat4();

		vkCmdPushConstants(commandBuffer, pipelineLayout,
		                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		                   0, sizeof(SimplePushCOnstantData), &push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
	}
}

} // namespace lve
