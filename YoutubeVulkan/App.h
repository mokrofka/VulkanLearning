#pragma once
#include "Device.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Window.h"

// std
#include <memory>

class App {
	static constexpr i32 WIDTH  = 1000;
	static constexpr i32 HEIGHT = 1000;

	Window    window{WIDTH, HEIGHT, L"Hello Vulkan"};
	Device    device{window};
	SwapChain swapChain{device, window.getExtent()};
	
	std::unique_ptr<Pipeline>    pipeline;
	VkPipelineLayout             pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;

	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffer();
	void drawFrame();

	/*Pipeline _Pipeline{
		_Device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv",
		Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
	};*/

public:
	App();
	~App();

	void run();

	App(const App&)            = delete;
	App& operator=(const App&) = delete;
};
