#pragma once
#include "defines.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_TO_ONE
#include <glm/glm.hpp>

class LveCamera {
	glm::mat4 projectionMatrix{1.0f};
	glm::mat4 viewMatrix{1.0f};
	glm::mat4 inverseViewMatrix{1.0f};

public:
	void setOrthographicProjection(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far);

	void setPerspectiveProjection(f32 fovy, f32 aspect, f32 near, f32 far);

	void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
	void setViewTarget(glm::vec3 target, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
	void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

	glm::mat4 getProjection()  const { return projectionMatrix;  }
	glm::mat4 getView()        const { return viewMatrix;        }
	glm::mat4 getInverseView() const { return inverseViewMatrix; }
	glm::vec3 getPosition()    const { return glm::vec3(inverseViewMatrix[3]); }
};
