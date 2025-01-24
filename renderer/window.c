#include "renderer/window.h"

#include "core/core.h"

#include "core/debug.h"
#include "renderer/context.h"

#include "platform/window.h"

// TODO: We might not need this
// #if defined(WEB_ENABLED)
// #include "platform/web/webgl.h"
// #endif // WEB_ENABLED

// Implemented in main because use of the renderer is required.

struct LSWindow {
	PlatformWindow *platform_window;

	Context *context;

	InputManager *input_manager;
};

LSWindow *renderer_create_window(WindowConfig config) {
	if (renderer_get_backend() == RENDERER_BACKEND_NONE) {
		ls_log(LOG_LEVEL_WARNING, "Cannot create window with renderer backend NONE\n");
		return NULL;
	}

	const LSCore *core = renderer_get_core();

	const OS *os = core_get_os(core);
	LS_ASSERT(os);

	LSWindow *window = ls_malloc(sizeof(LSWindow));
	window->platform_window = platform_create_window(os_get_platform_os(os), config, window);
	window->input_manager = core_get_input_manager(core);

	window->context = renderer_context_create(window);
	window_make_current(window);
	window_swap_buffers(window);

	input_handle_window_open(window->input_manager, window);

	return window;
}

void window_destroy(LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	if (window->context) {
		renderer_context_destroy(window->context);
	}

	platform_destroy_window(window->platform_window);
	ls_free(window);
}

LSNativeWindow window_get_native_window(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	return platform_window_get_native_window(window->platform_window);
}

void window_poll(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	input_set_active_window(window->input_manager, window);
	platform_window_poll(window->platform_window);
	input_set_active_window(window->input_manager, NULL);
}

void window_set_title(LSWindow *window, String title) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_set_title(window->platform_window, title);
}

void window_set_fullscreen(LSWindow *window, bool fullscreen) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_set_fullscreen(window->platform_window, fullscreen);
}

void window_set_min_size(LSWindow *window, Vector2u size) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_set_min_size(window->platform_window, size);
}

Vector2u window_get_min_size(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	return platform_window_get_min_size(window->platform_window);
}

void window_set_max_size(LSWindow *window, Vector2u size) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_set_max_size(window->platform_window, size);
}

Vector2u window_get_max_size(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	return platform_window_get_max_size(window->platform_window);
}

void window_set_size(LSWindow *window, Vector2u size) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_set_size(window->platform_window, size);
}

Vector2u window_get_size(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	return platform_window_get_size(window->platform_window);
}

void window_make_current(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	renderer_context_make_current(window->context);
	renderer_set_active_window(window);
}

void window_detach(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	renderer_context_detach(window->context);
	renderer_set_active_window(NULL);
}

void window_swap_buffers(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	renderer_context_swap_buffers(window->context);
}

void window_show(LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_show(window->platform_window);
}

void window_hide(LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	platform_window_hide(window->platform_window);
}

bool window_is_visible(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	return platform_window_is_visible(window->platform_window);
}

bool window_is_fullscreen(const LSWindow *window) {
	LS_ASSERT(window);
	LS_ASSERT(window->platform_window);

	return platform_window_is_fullscreen(window->platform_window);
}