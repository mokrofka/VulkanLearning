#include "LveGameObject.h"

glm::mat4 TransformComponent::mat4() {
	auto transform = glm::translate(glm::mat4(1.0f), translation);
	transform      = glm::rotate(transform, rotation.y, {0.0, -1.0, 0.0});
	transform      = glm::rotate(transform, rotation.x, {1.0, 0.0, 0.0});
	transform      = glm::rotate(transform, rotation.z, {0.0, 0.0, 1.0});
	transform      = glm::scale(transform, scale);

	return transform;
}

glm::mat3 TransformComponent::normalMatrix() {
	auto transform = glm::translate(glm::mat4(1.0f), translation);
	transform      = glm::rotate(transform, rotation.y, {0.0, -1.0, 0.0});
	transform      = glm::rotate(transform, rotation.x, {1.0, 0.0, 0.0});
	transform      = glm::rotate(transform, rotation.z, {0.0, 0.0, 1.0});
	transform      = glm::scale(transform, scale);
	return transform;
}

LveGameObject LveGameObject::makePointLight(f32 intensity, f32 radius, glm::vec3 color) {
	LveGameObject gameObj              = createGameObject();
	gameObj.color                      = color;
	gameObj.transform.scale.x          = radius;
	gameObj.pointLIght                 = std::make_unique<PointLightComponent>();
	gameObj.pointLIght->lightIntensity = intensity;
	return gameObj;
}
