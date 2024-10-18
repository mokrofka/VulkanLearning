#pragma once
#include "defines.h"

#include "LveWindow.h"
#include "LveDevice.h"
#include "LveSwapChain.h"

// std

#include <assert.h>
#include <memory>

class LveRenderer {
	LveWindow& Window;
	LveDevice& Device;
	std::unique_ptr<LveSwapChain> SwapChain;
	std::vector<VkCommandBuffer>  commandBuffers;

	u32 currentImageIndex;
	b8  isFrameStarted{};
	i32 currentFrameIndex{};

	void createCommandBuffer();
	void freeCommandBufers();
	void recreateSwapChain();

public:
	LveRenderer(LveWindow& window, LveDevice& device);
	~LveRenderer();

	VkRenderPass getSwapChainRenderPass()const { return SwapChain->getRenderPass(); }
	f32 getAspectRatio() const { return SwapChain->extentAspectRatio(); }
	b8 isFrameProgress() const { return isFrameStarted; }
	VkCommandBuffer getCurrentCommandBuffer() const {
		assert(isFrameProgress() && "Cannot get command buffer when frame not in progress");
		return commandBuffers[currentFrameIndex];
	}

	i32 getFrameIndex() const {
		assert(isFrameProgress() && "Cannot get frame index when frame not in progress");
		return  currentFrameIndex;
	}

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	LveRenderer(const LveRenderer&)            = delete;
	LveRenderer& operator=(const LveRenderer&) = delete;
};

