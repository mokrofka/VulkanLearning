#pragma once
#include "defines.h"

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

// std

#include <assert.h>
#include <memory>

namespace lve {


	class LveRenderer {
		LveWindow& lveWindow;
		lve::LveDevice& lveDevice;
		std::unique_ptr<lve::LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer>       commandBuffers;

		u32 currentImageIndex;
		b8  isFrameStarted{};
		i32 currentFramIndex{};

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

	public:
		LveRenderer(LveWindow& window, lve::LveDevice& device);
		~LveRenderer();

		VkRenderPass getSwapChainRenderPass()const { return lveSwapChain->getRenderPass(); }
		f32 getAspectRatio() const { return lveSwapChain->extentAspectRatio(); }
		b8 isFrameProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameProgress() && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFramIndex];
		}

		i32 getFrameIndex() const {
			assert(isFrameProgress() && "Cannot get frame index when frame not in progress");
			return  currentFramIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator=(const LveRenderer&) = delete;
	};

}