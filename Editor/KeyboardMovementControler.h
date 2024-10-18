#pragma once
#include "App.h"
#include "defines.h"

class KeyboardMovementControler {
public:
	struct KeyMappings {
		i32 moveLeft     = 'A';
		i32 moveRight    = 'D';
		i32 moveForward  = 'W';
		i32 moveBackward = 'S';
		i32 moveUp       = 'E';
		i32 moveDown     = 'Q';
		i32 lookLeft     = VK_LEFT;
		i32 lookRight    = VK_RIGHT;
		i32 lookUp       = VK_UP;
		i32 lookDown     = VK_DOWN;
	};

	void moveInPlaneXZ(f32 dt, LveGameObject& gameObject);

	KeyMappings keys{};
	f32 moveSpeed{3.0f};
	f32 lookSpeed{1.5f};

	inline static f32 deltaX{};
	inline static f32 deltaY{};
};
