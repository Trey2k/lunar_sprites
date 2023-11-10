#include "platform/windows/window.h"
#include "platform/windows/keymap.h"

#include "core/core.h"
#include <windows.h>
#include <windowsx.h>

static const char LS_WIN_CLASS_NAME[] = "LSWindowClass";

static int64 window_procedure(HWND native_window, uint32 message, uint64 w_param, int64 l_param);

PlatformWindow *platform_create_window(const PlatformOS *os, String title, int32 width, int32 height) {
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = &window_procedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = LS_WIN_CLASS_NAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		ls_log_fatal("Failed to register window class!\n");
		return NULL;
	}

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

static int64 window_procedure(HWND native_window, uint32 message, uint64 w_param, int64 l_param) {
	switch (message) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_KEYDOWN: {
			core_handle_press(win_map_key(w_param));
			return 0;
		} break;

		case WM_KEYUP: {
			core_handle_release(win_map_key(w_param));
			return 0;
		} break;

		case WM_MOUSEMOVE: {
			core_handle_mouse_move((Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_LBUTTONDOWN: {
			core_handle_mouse_press(LS_MOUSE_BUTTON_LEFT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_RBUTTONDOWN: {
			core_handle_mouse_press(LS_MOUSE_BUTTON_RIGHT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_MBUTTONDOWN: {
			core_handle_mouse_press(LS_MOUSE_BUTTON_MIDDLE,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_LBUTTONUP: {
			core_handle_mouse_release(LS_MOUSE_BUTTON_LEFT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_RBUTTONUP: {
			core_handle_mouse_release(LS_MOUSE_BUTTON_RIGHT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_MBUTTONUP: {
			core_handle_mouse_release(LS_MOUSE_BUTTON_MIDDLE,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		default:
			return DefWindowProc(native_window, message, w_param, l_param);
	};
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