#include "renderer/window.h"

#include "core/core.h"

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
};

LSWindow *renderer_create_window(WindowConfig config) {
	if (renderer_get_backend() == RENDERER_BACKEND_NONE) {
		ls_log(LOG_LEVEL_WARNING, "Cannot create window with renderer backend NONE\n");
		return NULL;
	}

	const OS *os = ls_get_os();
	LS_ASSERT(os);

	LSWindow *window = ls_malloc(sizeof(LSWindow));
	window->platform_window = platform_create_window(os_get_platform_os(os), config);

	window->context = renderer_context_create(window);
	window_make_current(window);
	window_swap_buffers(window);

	return window;
}

void window_destroy(LSWindow *window) {
	LS_ASSERT(window);
	if (window->context) {
		renderer_context_destroy(window->context);
	}

	platform_destroy_window(window->platform_window);
	ls_free(window);
}

LSNativeWindow window_get_native_window(const LSWindow *window) {
	LS_ASSERT(window);

	return platform_window_get_native_window(window->platform_window);
}

void window_poll(const LSWindow *window) {
	LS_ASSERT(window);

	core_set_active_window(window);
	platform_window_poll(window->platform_window);
	core_set_active_window(NULL);
}

void window_set_title(LSWindow *window, String title) {
	LS_ASSERT(window);

	platform_window_set_title(window->platform_window, title);
}

void window_set_size(LSWindow *window, int32 width, int32 height) {
	LS_ASSERT(window);

	platform_window_set_size(window->platform_window, width, height);
}

void window_make_current(const LSWindow *window) {
	LS_ASSERT(window);

	renderer_context_make_current(window->context);
}

void window_swap_buffers(const LSWindow *window) {
	LS_ASSERT(window);

	renderer_context_swap_buffers(window->context);
}

void window_show(LSWindow *window) {
	LS_ASSERT(window);

	platform_window_show(window->platform_window);
}

void window_hide(LSWindow *window) {
	LS_ASSERT(window);

	platform_window_hide(window->platform_window);
}

bool window_is_visible(const LSWindow *window) {
	LS_ASSERT(window);

	return platform_window_is_visible(window->platform_window);
}