#pragma once
#include "LveBuffer.h"
#include "LveDevice.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>

class LveModel {
public:
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription>   getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		b8 operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
		}
	};

	struct Builder {
		std::vector<Vertex> vertices{};
		std::vector<u32> indices{};

		void loadModel(const std::string& filepath);
	};

private:
	LveDevice& lveDevice;

	std::unique_ptr<LveBuffer> vertexBuffer;
	u32 vertexCount;

	b8 hasIndexBuffer = false;
	std::unique_ptr<LveBuffer> indexBuffer;
	u32 indexCount;

	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<u32>& indices);

public:

	LveModel(LveDevice &Device, const Builder& builder);
	~LveModel();

	static std::unique_ptr<LveModel> createModelFromFile(LveDevice& device, const std::string& filepath);

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

	LveModel(const LveModel&)            = delete;
	LveModel& operator=(const LveModel&) = delete;
};
