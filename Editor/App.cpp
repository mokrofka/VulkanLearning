#include "App.h"
#include "KeyboardMovementControler.h"
#include "systems/point_light_system.h"
#include "systems/SimpleRenderSystem.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <glm/gtx/dual_quaternion.hpp>
#include <chrono>
#include <stdexcept>

App::App() {
	globalPool = LveDescriptorPool::Builder(lveDevice)
	             .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
	             .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
	             .build();
	loadGameObjects();
}

App::~App() {}

void App::run() {
	std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (i32 i = 0; i < uboBuffers.size(); ++i) {
		uboBuffers[i] = std::make_unique<LveBuffer>(
			lveDevice,
			sizeof(GlobalUbo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		uboBuffers[i]->map();
	}

	auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
	                      .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
	                      .build();
	
	std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (i32 i = 0; i < globalDescriptorSets.size(); ++i) {
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		LveDescriptorWriter(*globalSetLayout, *globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
	}
	
	SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass(), {globalSetLayout->getDescriptorSetLayout()}};
	PointLightSystem pointLightSystem{lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};

	LveCamera camera{};

	auto viewerObject = LveGameObject::createGameObject();
	viewerObject.transform.translation.z = -2.5f;
	KeyboardMovementControler cameraController{};

	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!lveWindow.shouldClose()) {
		LveWindow::update();

		auto newTime   = std::chrono::high_resolution_clock::now();
		f32  frameTime = std::chrono::duration<f32, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime    = newTime;

		cameraController.moveInPlaneXZ(frameTime, viewerObject);
		camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		f32 aspect = lveRenderer.getAspectRatio();
		camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

		if (auto commandBuffer = lveRenderer.beginFrame()) {
			i32 frameIndex = lveRenderer.getFrameIndex();
			FrameInfo frameInfo{
				frameIndex,
				frameTime,
				commandBuffer,
				camera,
				{globalDescriptorSets[frameIndex]},
				gameObjects
			};

			// update
			GlobalUbo ubo{};
			ubo.projection  = camera.getProjection();
			ubo.view        = camera.getView();
			ubo.inverseView = camera.getInverseView();
			pointLightSystem.update(frameInfo, ubo);
			uboBuffers[frameIndex]->writeToIndex(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			lveRenderer.beginSwapChainRenderPass(commandBuffer);

			simpleRenderSystem.renderGameObjects(frameInfo);
			pointLightSystem.render(frameInfo);

			lveRenderer.endSwapChainRenderPass(commandBuffer);
			lveRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(lveDevice.device);
}

std::unique_ptr<LveModel> createCubeModel(LveDevice& device, glm::vec3 offset) {
	LveModel::Builder modelBuilder{};
	modelBuilder.vertices = {
		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
	};
	for (auto& v : modelBuilder.vertices) {
		v.position += offset;
	}

	modelBuilder.indices = {
		0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9,
		12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21
	};
	return std::make_unique<LveModel>(device, modelBuilder);
}

void App::loadGameObjects() {
	// std::shared_ptr<vk_Model> Model = createCubeModel(Device, {0.0f, 0.0f, 0.0f});

	std::shared_ptr<LveModel> Model = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");

	auto flatVase  = LveGameObject::createGameObject();
	flatVase.model = Model;
	flatVase.transform.translation = {-0.5f, 0.5, 0.0f};
	flatVase.transform.scale = glm::vec3(3.0f, 1.5f, 3.0f);
	gameObjects.emplace(flatVase.getId(), std::move(flatVase));

	Model = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
	auto smoothVase  = LveGameObject::createGameObject();
	smoothVase.model = Model;
	smoothVase.transform.translation = {0.5f, 0.5, 0.0f};
	smoothVase.transform.scale = glm::vec3(3.0f, 1.5f, 3.0f);
	gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

	Model = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
	auto floor  = LveGameObject::createGameObject();
	floor.model = Model;
	floor.transform.translation = {0.0f, 0.5, 0.0f};
	floor.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
	gameObjects.emplace(floor.getId(), std::move(floor));

	 std::vector<glm::vec3> lightColors{
      {1.f, .1f, .1f},
      {.1f, .1f, 1.f},
      {.1f, 1.f, .1f},
      {1.f, 1.f, .1f},
      {.1f, 1.f, 1.f},
      {1.f, 1.f, 1.f}
  };

	for (i32 i = 0; i < lightColors.size(); ++i) {
		auto pointLight  = LveGameObject::makePointLight(0.2f);
		pointLight.color = lightColors[i];
		auto rotateLight = glm::rotate(
			glm::mat4(1.0f), (i * glm::two_pi<f32>()) / lightColors.size(), {0.0f, -1.0f, 0.0f});
		pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
		gameObjects.emplace(pointLight.getId(), std::move(pointLight));
	}
}
