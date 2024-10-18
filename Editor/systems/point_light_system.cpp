#include "point_light_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std
#include <glm/ext/quaternion_geometric.hpp>
#include <map>
#include <stdexcept>

struct PointLightPushConstantds {
	glm::vec4 position{};
	glm::vec4 color;
	f32 radius;
};

PointLightSystem::PointLightSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) :
	Device(device) {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
	vkDestroyPipelineLayout(Device.device, pipelineLayout, nullptr);
}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset     = 0;
	pushConstantRange.size       = sizeof(PointLightPushConstantds);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount         = (u32)descriptorSetLayouts.size();
	pipelineLayoutInfo.pSetLayouts            = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges    = &pushConstantRange;
	if (vkCreatePipelineLayout(Device.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
	assert(pipelineLayout != nullptr && "Cannot create pipeline before swapchain");

	PipelineConfigInfo pipelineConfig{};
	LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
	LvePipeline::enableAlphaBlending(pipelineConfig);
	pipelineConfig.attributeDescriptions.clear();
	pipelineConfig.bindingDescriptions.clear();
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	Pipeline = std::make_unique<LvePipeline>(Device, "point_light.vert", "point_light.frag", pipelineConfig);
}

void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
	auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime, {0.0f, -1.0f, 0.0f});

	i32 lightIndex = 0;
	for (auto& kv : frameInfo.gameObjects) {
		auto& obj = kv.second;
		if (obj.pointLIght == nullptr) continue;
		assert(lightIndex < MAX_LIGHTS && "Point light exceed maximum specified");

		// update light position
		obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

		// copy light to ubo
		ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
		ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLIght->lightIntensity);

		lightIndex += 1;
	}
	ubo.numLights = lightIndex;
}

void PointLightSystem::render(FrameInfo& frameInfo) {
	// sort lights
	std::map<f32, LveGameObject::id_t> sorted;
	for (auto& kv : frameInfo.gameObjects) {
		auto& obj = kv.second;
		if (obj.pointLIght == nullptr) continue;

		// calculate distance
		auto offset        = frameInfo.camera.getPosition() - obj.transform.translation;
		f32  disSquared    = glm::dot(offset, offset);
		sorted[disSquared] = obj.getId();
	}

	Pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0, 1,
		&frameInfo.globalDescriptorSet[0],
		0, nullptr
	);

	// iterate through sorted lights in reverse order
	for (auto& it = sorted.rbegin(); it != sorted.rend(); ++it) {
		auto& obj = frameInfo.gameObjects.at(it->second);

		PointLightPushConstantds push{};
		push.position = glm::vec4(obj.transform.translation, 1.0f);
		push.color    = glm::vec4(obj.color, obj.pointLIght->lightIntensity);
		push.radius   = obj.transform.scale.x;

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(PointLightPushConstantds),
			&push
		);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}
