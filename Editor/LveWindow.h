#pragma once
#include "defines.h"

#include <vulkan/vulkan_core.h>

#include <string>
#include <Windows.h>

class LveWindow {
	i32 width;
	i32 height;
	
	b8 framebufferResized = false;
	std::wstring windowName;

	inline static HWND window = nullptr;

public:
	static void framebufferResizeCallback(HWND window, i32 width, i32 height);
	inline static b8 running = true;

	LveWindow(i32 w, i32 h, std::wstring name);
	~LveWindow();

	void initWindow();
	VkExtent2D getExtent() { return {(u32)width, (u32)height}; }
	b8 shouldClose() { return !running; }
	b8 isWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }

	static void update();

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	LveWindow(const LveWindow&)            = delete;
	LveWindow& operator=(const LveWindow&) = delete;
};
