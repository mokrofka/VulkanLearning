#pragma once
#include "defines.h"
#include "LveModel.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

struct TransformComponent {
	glm::vec3 translation{};
	glm::vec3 scale{1.0f, 1.0f, 1.0f};
	glm::vec3 rotation{};

	// Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
	// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
	f32 scaleValue{};
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

struct PointLightComponent {
	f32 lightIntensity = 1.0f;
};

class LveGameObject {
public:
	typedef u32 id_t;
	typedef std::unordered_map<id_t, LveGameObject> Map;
	id_t        id;

private:
	LveGameObject(id_t objId) : id{objId} {}

public:

	static LveGameObject createGameObject() {
		static id_t currentId = 0;
		return LveGameObject{currentId++};
	}

	id_t getId() { return id; }

	glm::vec3          color{};
	TransformComponent transform{};

	std::shared_ptr<LveModel>            model{};
	std::unique_ptr<PointLightComponent> pointLIght = nullptr;

	static LveGameObject makePointLight(
		f32 intensity = 10.0f, f32 radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

	LveGameObject(const LveGameObject&)            = delete;
	LveGameObject& operator=(const LveGameObject&) = delete;
	LveGameObject(LveGameObject&&)                 = default;
	LveGameObject& operator=(LveGameObject&&)      = default;
};
