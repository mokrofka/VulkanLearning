#pragma once

#include "Device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

class SwapChain {
	VkSwapchainKHR swapChain;
	VkFormat       swapChainImageFormat;
	VkExtent2D     swapChainExtent;

	Device& device;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;

	std::vector<VkImage>        depthImages;
	std::vector<VkDeviceMemory> depthImageMemorys;
	std::vector<VkImageView>    depthImageViews;

	std::vector<VkImageView>    swapChainImageViews;
	std::vector<VkImage>        swapChainImages;
	VkExtent2D windowExtent;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence>     inFlightFences;
	std::vector<VkFence>     imagesInFlight;

	u64 currentFrame = 0;

	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	void createRenderPass();
	void createFramebuffers();
	void createSyncObjects();

	// Helper functions
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR   chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D         chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

public:
	constexpr static i32 MAX_FRAMES_IN_FLIGHT = 2;

	SwapChain(Device& deviceRef, VkExtent2D windowExtent);
	~SwapChain();

	SwapChain(const SwapChain&)      = delete;
	void operator=(const SwapChain&) = delete;

	VkFramebuffer getFrameBuffer(i32 index) { return swapChainFramebuffers[index]; }
	VkRenderPass  getRenderPass()           { return renderPass;                   }
	VkImageView   getImageView(i32 index)   { return swapChainImageViews[index];   }
	VkFormat      getSwapChainImageFormat() { return swapChainImageFormat;         }
	VkExtent2D    getSwapChainExtent()      { return swapChainExtent;              }
	u64 imageCount()                        { return swapChainImages.size();       }
	u32 width()                             { return swapChainExtent.width;        }
	u32 height()                            { return swapChainExtent.height;       }

	f32 extentAspectRatio() {
		return (f32)swapChainExtent.width / (f32)swapChainExtent.height;
	}

	VkFormat findDepthFormat();

	VkResult acquireNextImage(u32* imageIndex);
	VkResult submitCommandBuffers(const VkCommandBuffer* buffers, u32* imageIndex);
};
