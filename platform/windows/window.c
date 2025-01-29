#include "platform/windows/window.h"
#include "platform/window.h"
#include "platform/windows/os.h"

#include "core/core.h"
#include <windows.h>
#include <windowsx.h>

PlatformWindow *platform_create_window(WindowConfig config, LSWindow *parent) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));

	window->title = config.title;
	window->size = config.size;
	window->min_size = config.min_size;
	window->max_size = config.max_size;
	window->hidden = true;
	window->fullscreen = false;
	window->parent = parent;
	window->should_close = false;

	window->window = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			LS_WIN_CLASS_NAME,
			window->title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, window->size.x, window->size.y,
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
		SetWindowPos(window->window, NULL, 0, 0, window->size.x, window->size.y,
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

void platform_window_set_min_size(PlatformWindow *window, Vector2u size) {
	window->min_size = size;
}

Vector2u platform_window_get_min_size(const PlatformWindow *window) {
	return window->min_size;
}

void platform_window_set_max_size(PlatformWindow *window, Vector2u size) {
	window->max_size = size;
}

Vector2u platform_window_get_max_size(const PlatformWindow *window) {
	return window->max_size;
}

void platform_window_set_size(PlatformWindow *window, Vector2u size) {
	RECT rect;
	GetWindowRect(window->window, &rect);
	MoveWindow(window->window, rect.left, rect.top, size.x, size.y, TRUE);
	window->size = size;
}

Vector2u platform_window_get_size(const PlatformWindow *window) {
	RECT rect;
	GetWindowRect(window->window, &rect);
	// sub window dimensions
	return vec2u(rect.right - rect.left - 16, rect.bottom - rect.top - 39);
}

bool platform_window_is_visible(const PlatformWindow *window) {
	return !window->hidden;
}

bool platform_window_is_fullscreen(const PlatformWindow *window) {
	return window->fullscreen;
}

bool platform_window_should_close(const PlatformWindow *window) {
	return window->should_close;
}