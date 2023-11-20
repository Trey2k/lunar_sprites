#include "platform/windows/os.h"
#include "platform/windows/keymap.h"

#include "core/core.h"

static int64 window_procedure(HWND native_window, uint32 message, uint64 w_param, int64 l_param);

PlatformOS *platform_create_os(InputManager *input_manager) {
	LS_ASSERT(input_manager);

	PlatformOS *os = ls_malloc(sizeof(PlatformOS));

	os->window_class.cbSize = sizeof(WNDCLASSEX);
	os->window_class.style = 0;
	os->window_class.lpfnWndProc = &window_procedure;
	os->window_class.cbClsExtra = 0;
	os->window_class.cbWndExtra = 0;
	os->window_class.hInstance = NULL;
	os->window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	os->window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	os->window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	os->window_class.lpszMenuName = NULL;
	os->window_class.lpszClassName = LS_WIN_CLASS_NAME;
	os->window_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	os->input_manager = input_manager;

	if (!RegisterClassEx(&os->window_class)) {
		ls_log_fatal("Failed to register window class!\n");
		return NULL;
	}

	return os;
}

void platform_destroy_os(PlatformOS *os) {
	ls_free(os);
}

LSNativeDisplayType platform_get_native_display(const PlatformOS *os) {
	return CreateCompatibleDC(NULL);
}

static int64 window_procedure(HWND native_window, uint32 message, uint64 w_param, int64 l_param) {
	PlatformWindow *window = (PlatformWindow *)GetWindowLongPtr(native_window, GWLP_USERDATA);
	LS_ASSERT(window);

	switch (message) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_KEYDOWN: {
			input_manager_handle_press(window->input_manager, win_map_key(w_param));
			return 0;
		} break;

		case WM_KEYUP: {
			input_manager_handle_release(window->input_manager, win_map_key(w_param));
			return 0;
		} break;

		case WM_MOUSEMOVE: {
			input_manager_handle_mouse_move(window->input_manager,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_LBUTTONDOWN: {
			input_manager_handle_mouse_press(window->input_manager, LS_MOUSE_BUTTON_LEFT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_RBUTTONDOWN: {
			input_manager_handle_mouse_press(window->input_manager, LS_MOUSE_BUTTON_RIGHT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_MBUTTONDOWN: {
			input_manager_handle_mouse_press(window->input_manager, LS_MOUSE_BUTTON_MIDDLE,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_LBUTTONUP: {
			input_manager_handle_mouse_release(window->input_manager, LS_MOUSE_BUTTON_LEFT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_RBUTTONUP: {
			input_manager_handle_mouse_release(window->input_manager, LS_MOUSE_BUTTON_RIGHT,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_MBUTTONUP: {
			input_manager_handle_mouse_release(window->input_manager, LS_MOUSE_BUTTON_MIDDLE,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_MOUSELEAVE: {
			input_manager_handle_mouse_leave(window->input_manager,
					(Vector2i){ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			return 0;
		} break;

		case WM_CLOSE: {
			input_manager_handle_window_close(window->input_manager);
			return 0;
		} break;

		default:
			return DefWindowProc(native_window, message, w_param, l_param);
	};
}