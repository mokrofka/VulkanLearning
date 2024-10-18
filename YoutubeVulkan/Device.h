#pragma once

#include "Window.h"

#include "vulkan/vulkan_core.h"
#include <vector>

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR        capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR>   presentModes;
};

struct QueueFamilyIndices {
	u32 graphicsFamily;
	u32 presentFamily;
	b8  graphicsFamilyHasValue = false;
	b8  presentFamilyHasValue  = false;
	b8  isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class Device {
	VkInstance               instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE;
	Window&                  window;

	const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
	const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();

	// helper functions
	b8 isDeviceSuitable(VkPhysicalDevice device);
	b8 checkValidationLayerSupport();
	b8 checkDeviceExtensionSupport(VkPhysicalDevice device);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void hasGflwRequiredInstanceExtensions();
	std::vector<const char*> getRequiredExtensions();
	QueueFamilyIndices       findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails  querySwapChainSupport(VkPhysicalDevice device);

public:
#ifdef NDEBUG
	const b8 enableValidationLayers = false;
#else
	const b8 enableValidationLayers = true;
#endif
	VkDevice      device;
	VkSurfaceKHR  surface;
	VkQueue       graphicsQueue;
	VkQueue       presentQueue;
	VkCommandPool commandPool;
	VkPhysicalDeviceProperties properties;

	Device(Window& window);
	~Device();

	SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
	QueueFamilyIndices      findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
	VkFormat                findSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	u32                     findMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);

	// Buffer Helper Functions
	void createImageWithInfo  (const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties,
						   VkImage& image, VkDeviceMemory& imageMemory);
	void createBuffer         (VkDeviceSize          size, VkBufferUsageFlags usage,
	                           VkMemoryPropertyFlags properties, VkBuffer&    buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer           (VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage    (VkBuffer buffer, VkImage image, u32 width, u32 height, u32 layerCount);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	
	// Not copyable or movable
	Device(const Device&)         = delete;
	void operator=(const Device&) = delete;
	Device(Device&&)              = delete;
	Device& operator=(Device&&)   = delete;

};
