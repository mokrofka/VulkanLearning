#pragma once

#include "LveCamera.h"

// lib

#include "LveGameObject.h"

#include <vulkan/vulkan.h>

#define MAX_LIGHTS 10

struct PointLight {
	glm::vec4 position{}; // w ignore
	glm::vec4 color{}; // w is intensity
};

struct GlobalUbo {
	glm::mat4 projection{1.0f};
	glm::mat4 view{1.0f};
	glm::mat4 inverseView{1.0};
	glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f};
	PointLight pointLights[MAX_LIGHTS];
	i32 numLights;
};

struct FrameInfo {
	i32                          frameIndex;
	f32                          frameTime;
	VkCommandBuffer              commandBuffer;
	LveCamera&                   camera;
	const std::vector<VkDescriptorSet>& globalDescriptorSet;
	LveGameObject::Map& gameObjects;
};
