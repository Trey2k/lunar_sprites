#include "platform/windows/window.h"
#include "platform/windows/os.h"

#include "core/core.h"
#include <windows.h>
#include <windowsx.h>

PlatformWindow *platform_create_window(const PlatformOS *os, String title, int32 width, int32 height) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));
	window->window = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			LS_WIN_CLASS_NAME,
			title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, width, height,
			NULL, NULL, NULL, NULL);
	if (!window->window) {
		ls_log_fatal("Failed to create window!\n");
		return NULL;
	}

	SetWindowLongA(window->window, GWLP_USERDATA, (LONG_PTR)window);

	ShowWindow(window->window, SW_SHOW);
	UpdateWindow(window->window);

	return window;
}

void platform_destroy_window(PlatformWindow *window) {
	DestroyWindow(window->window);
	ls_free(window);
}

LSNativeWindow platform_get_window_handle(const PlatformWindow *window) {
	return window->window;
}

void platform_window_poll(const PlatformWindow *window) {
	MSG msg;

	while (PeekMessage(&msg, window->window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window) {
	return window->window;
}

void platform_window_set_title(const PlatformWindow *window, String title) {
	SetWindowText(window->window, title);
}

void platform_window_set_size(const PlatformWindow *window, int32 width, int32 height) {
	RECT rect;
	GetWindowRect(window->window, &rect);
	MoveWindow(window->window, rect.left, rect.top, width, height, TRUE);
}