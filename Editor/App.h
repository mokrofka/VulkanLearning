#pragma once
#include "LveGameObject.h"
#include "LveRenderer.h"
#include "LveWindow.h"
#include "LveDevice.h"
// std

#include "LveDescriptor.h"

#include <memory>

class App {
	static constexpr i32 WIDTH  = 800;
	static constexpr i32 HEIGHT = 600;

	LveWindow   lveWindow{WIDTH, HEIGHT, L"Hello Vulkan"};
	LveDevice   lveDevice{lveWindow};
	LveRenderer lveRenderer{lveWindow, lveDevice};

	std::unique_ptr<LveDescriptorPool> globalPool{};
	LveGameObject::Map gameObjects;

	void loadGameObjects();

public:
	App();
	~App();

	void run();

	App(const App&)            = delete;
	App& operator=(const App&) = delete;
};
