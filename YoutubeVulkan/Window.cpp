#include "Window.h"

#include <vulkan/vulkan_win32.h>

#include <stdexcept>

LRESULT windProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp) {
	LRESULT  res = 0;

	switch (msg) {
	case WM_CLOSE: 
		Window::running = false;
		break;
	
	case WM_DESTROY:
		Window::running = false;
		break;
	default:
		res = DefWindowProc(wind, msg, wp, lp);
		break;
	}
	return res;
}

Window::Window(i32 w, i32 h, std::wstring name): width(w), height(h), windowName(name) {
	initWindow();
}

Window::~Window() {
	DestroyWindow(window);
}

void Window::initWindow() {
	WNDCLASS wc = {};

	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hInstance     = GetModuleHandle(0);
	wc.lpszClassName = L"EpicWindowClass";
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = windProc;

	if (!RegisterClass(&wc)) { return; };

	window = CreateWindow(
		wc.lpszClassName,
		windowName.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0, 0, GetModuleHandle(0), 0
	);

	ShowWindow(window,SW_SHOW);
}

void Window::update() {
	MSG msg;

	while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hwnd                        = window;
	surfaceInfo.hinstance                   = GetModuleHandleA(0);
	if(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, 0, surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}