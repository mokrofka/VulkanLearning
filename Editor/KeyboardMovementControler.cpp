#include "KeyboardMovementControler.h"

#include <random>

void KeyboardMovementControler::moveInPlaneXZ(f32 dt, LveGameObject& gameObject) {
	glm::vec3 rotate{0};
	if (0x8000 & GetAsyncKeyState(keys.lookRight)) rotate.y += 1.0f;
	if (0x8000 & GetAsyncKeyState(keys.lookLeft))  rotate.y -= 1.0f;
	if (0x8000 & GetAsyncKeyState(keys.lookUp))    rotate.x += 1.0f;
	if (0x8000 & GetAsyncKeyState(keys.lookDown))  rotate.x -= 1.0f;

	rotate.y += deltaX /** 0.01*/;
	rotate.x -= deltaY /** 0.01*/;

	if (glm::dot(rotate, rotate) > std::numeric_limits<f32>::epsilon()) {
		// gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		gameObject.transform.rotation += lookSpeed * 0.1f * dt * rotate;
	}

	deltaX = 0;
	deltaY = 0;

	gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
	gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<f32>());

	f32 yaw = gameObject.transform.rotation.y;
	const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
	const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
	const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

	glm::vec3 moveDir{0.0f};
	if (0x8000 & GetAsyncKeyState(keys.moveForward))  moveDir += forwardDir;
	if (0x8000 & GetAsyncKeyState(keys.moveBackward)) moveDir -= forwardDir;
	if (0x8000 & GetAsyncKeyState(keys.moveRight))    moveDir += rightDir;
	if (0x8000 & GetAsyncKeyState(keys.moveLeft))     moveDir -= rightDir;
	if (0x8000 & GetAsyncKeyState(keys.moveUp))       moveDir += upDir;
	if (0x8000 & GetAsyncKeyState(keys.moveDown))     moveDir -= upDir;

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<f32>::epsilon()) {
		gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}

}
