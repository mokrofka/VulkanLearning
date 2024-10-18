#include "lve_window.hpp"

#include <vulkan/vulkan_win32.h>

#include <stdexcept>

LRESULT windProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp) {
	LRESULT res = 0;

	switch (msg) {
	case WM_SIZE: {
		LveWindow::framebufferResizeCallback(wind, LOWORD(lp), HIWORD(lp));
		return 0;
	}

	// unintentionally turn off title bar and fix issues with minimize window and dymanic resizing
	/*case WM_NCCALCSIZE: {
		Window::framebufferResizeCallback(wind, LOWORD(lp), HIWORD(lp));
		return 0;
	}*/

	case WM_CLOSE: 
		LveWindow::running = false;
		break;
	
	case WM_DESTROY:
		LveWindow::running = false;
		break;
	default:
		res = DefWindowProc(wind, msg, wp, lp);
		break;
	}
	return res;
}

void LveWindow::framebufferResizeCallback(HWND window, i32 width, i32 height) {
	LveWindow* pThis = (LveWindow*)GetWindowLongPtr(HWND(window), GWLP_USERDATA);
	if (pThis != NULL) {
		pThis->framebufferResized = true;
		pThis->width              = width;
		pThis->height             = height;
	}
}

LveWindow::LveWindow(i32 w, i32 h, std::wstring name): width(w), height(h), windowName(name) {
	initWindow();
}

LveWindow::~LveWindow() {
	DestroyWindow(window);
}

void LveWindow::initWindow() {
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
		0, 0, GetModuleHandle(0), this
	);

	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this); // Set user data

	ShowWindow(window,SW_SHOW);
}

void LveWindow::update() {
	MSG msg;

	while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
	VkWin32SurfaceCreateInfoKHR surfaceInfo{};
	surfaceInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hwnd      = window;
	surfaceInfo.hinstance = GetModuleHandleA(0);
	if(vkCreateWin32SurfaceKHR(instance, &surfaceInfo, 0, surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
}