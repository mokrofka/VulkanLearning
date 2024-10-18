#include "SimpleRenderSystem.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std
#include <stdexcept>

struct SimplePushCOnstantData {
	// glm::mat4 transform{1.0f};
	glm::mat4 modelMatrix{1.0f};
};

SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass, const std::vector<VkDescriptorSetLayout>& globalSetLayout) :
	Device(device) {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
	vkDestroyPipelineLayout(Device.device, pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout(const std::vector<VkDescriptorSetLayout> globalSetLayout) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset     = 0;
	pushConstantRange.size       = sizeof(SimplePushCOnstantData);

	// std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	/*pipelineLayoutInfo.setLayoutCount         = (u32)descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts            = descriptorSetLayouts.data();*/
	pipelineLayoutInfo.setLayoutCount         = (u32)globalSetLayout.size();
	pipelineLayoutInfo.pSetLayouts            = globalSetLayout.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
	if (vkCreatePipelineLayout(Device.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before swapchain");

	PipelineConfigInfo pipelineConfig{};
	LvePipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	Pipeline = std::make_unique<LvePipeline>(Device, "simple_shader.vert", "simple_shader.frag", pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(FrameInfo& frameInfo) {
	Pipeline->bind(frameInfo.commandBuffer);
	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0, 1,
		&frameInfo.globalDescriptorSet[0],
		0, nullptr
	);

	for (auto& kv : frameInfo.gameObjects) {
		auto& obj = kv.second;
		if (obj.model == nullptr) continue;

		SimplePushCOnstantData push{};
		push.modelMatrix = obj.transform.mat4();

		vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
		                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		                   0, sizeof(SimplePushCOnstantData), &push);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}
}
