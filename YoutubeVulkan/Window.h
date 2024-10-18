#pragma once
#include "defines.h"

#include <vulkan/vulkan_core.h>

#include <string>
#include <Windows.h>

class Window {
	const i32    width;
	const i32    height;
	std::wstring windowName;

	inline static HWND window = nullptr;
public:
	inline static b8 running = true;

	Window(i32 w, i32 h, std::wstring name);
	~Window();

	void initWindow();
	b8 shouldClose() { return !running; }
	VkExtent2D getExtent() { return {(u32)width, (u32)height}; }

	static void update();

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	Window(const Window&)            = delete;
	Window& operator=(const Window&) = delete;
};

LRESULT CALLBACK windProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp);
