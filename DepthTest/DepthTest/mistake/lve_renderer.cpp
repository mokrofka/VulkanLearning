#include "lve_renderer.hpp"

// std
#include "lve_pipeline.hpp"

#include <array>
#include <stdexcept>

namespace lve {
	LveRenderer::LveRenderer(LveWindow& window, lve::LveDevice& device) :
		Window(window), Device(device) {
		recreateSwapChain();
		createCommandBuffer();
	}

	LveRenderer::~LveRenderer() {
		freeCommandBufers();
	}

	void LveRenderer::recreateSwapChain() {
		auto extent = Window.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = Window.getExtent();
			LveWindow::update(); // just Waits
		}
		vkDeviceWaitIdle(Device.device());

		if (SwapChain == nullptr) {
			SwapChain = std::make_unique<LveSwapChain>(Device, extent);
		} else {
			std::shared_ptr<LveSwapChain> oldSwapChain = std::move(SwapChain);

			SwapChain = std::make_unique<LveSwapChain>(Device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*SwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}

		// we'll come back to this in just a moment
	}

	void LveRenderer::createCommandBuffer() {
		commandBuffers.resize(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = Device.getCommandPool();
		allocInfo.commandBufferCount = (u32)commandBuffers.size();

		if (vkAllocateCommandBuffers(Device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void LveRenderer::freeCommandBufers() {
		vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), (u32)commandBuffers.size(), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer LveRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");
		auto result = SwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		return commandBuffer;
	}

	void LveRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Window.isWindowResized()) {
			Window.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFramIndex = (currentFramIndex + 1) % LveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void LveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beignSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = SwapChain->getRenderPass();
		renderPassInfo.framebuffer = SwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.01f, 0.01f, 0.01f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = (u32)clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (f32)SwapChain->getSwapChainExtent().width;
		viewport.height = (f32)SwapChain->getSwapChainExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 0.0f;
		VkRect2D scissor{{0, 0}, SwapChain->getSwapChainExtent()};
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}