#include "platform/windows/window.h"
#include "platform/window.h"
#include "platform/windows/os.h"

#include "core/core.h"
#include <windows.h>
#include <windowsx.h>

PlatformWindow *platform_create_window(const PlatformOS *os, WindowConfig config, const Renderer *renderer, const LSWindow *parent) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));

	window->title = config.title;
	window->width = config.size.x;
	window->height = config.size.y;
	window->hidden = true;
	window->fullscreen = false;
	window->parent = parent;

	window->input_manager = os->input_manager;

	window->window = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			LS_WIN_CLASS_NAME,
			window->title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, window->width, window->height,
			NULL, NULL, NULL, window);
	if (!window->window) {
		ls_log_fatal("Failed to create window!\n");
		return NULL;
	}

	SetWindowLongPtr(window->window, GWLP_USERDATA, (LONG_PTR)window);

	platform_window_set_fullscreen(window, config.fullscreen);

	if (!config.hidden) {
		platform_window_show(window);
	}

	UpdateWindow(window->window);

	return window;
}

void platform_destroy_window(PlatformWindow *window) {
	DestroyWindow(window->window);
	ls_free(window);
}

void platform_window_poll(PlatformWindow *window) {
	MSG msg;

	while (PeekMessage(&msg, window->window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window) {
	return window->window;
}

void platform_window_set_fullscreen(PlatformWindow *window, bool fullscreen) {
	if (window->fullscreen == fullscreen) {
		return;
	}

	if (fullscreen) {
		MONITORINFO monitor_info;
		monitor_info.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(MonitorFromWindow(window->window, MONITOR_DEFAULTTONEAREST), &monitor_info);

		SetWindowLong(window->window, GWL_STYLE, WS_POPUP);
		SetWindowPos(window->window, HWND_TOP, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
				monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
				monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	} else {
		SetWindowLong(window->window, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(window->window, NULL, 0, 0, window->width, window->height,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	window->fullscreen = fullscreen;
}

void platform_window_show(PlatformWindow *window) {
	if (!window->hidden) {
		return;
	}

	window->hidden = false;

	ShowWindow(window->window, SW_SHOW);
}

void platform_window_hide(PlatformWindow *window) {
	if (window->hidden) {
		return;
	}

	window->hidden = true;

	ShowWindow(window->window, SW_HIDE);
}

void platform_window_set_title(PlatformWindow *window, String title) {
	SetWindowText(window->window, title);
	window->title = title;
}

void platform_window_set_size(PlatformWindow *window, int32 width, int32 height) {
	RECT rect;
	GetWindowRect(window->window, &rect);
	MoveWindow(window->window, rect.left, rect.top, width, height, TRUE);
	window->width = width;
	window->height = height;
}

Vector2u platform_window_get_size(const PlatformWindow *window) {
	RECT rect;
	GetWindowRect(window->window, &rect);
	return vec2u(rect.right - rect.left, rect.bottom - rect.top);
}

bool platform_window_is_visible(const PlatformWindow *window) {
	return !window->hidden;
}

bool platform_window_is_fullscreen(const PlatformWindow *window) {
	return window->fullscreen;
}