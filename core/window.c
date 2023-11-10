#include "core/window.h"
#include "core/debug.h"
#include "core/memory.h"

#include "renderer/context.h"

#include "core/core.h"

struct LSWindow {
	PlatformWindow *platform_window;

	Context *context;
};

LSWindow *ls_create_window(String title, int32 width, int32 height) {
	if (renderer_get_backend() == RENDERER_BACKEND_NONE) {
		ls_log(LOG_LEVEL_WARNING, "Cannot create window with renderer backend NONE\n");
		return NULL;
	}

	const OS *os = ls_get_os();
	LS_ASSERT(os);

	LSWindow *window = ls_malloc(sizeof(LSWindow));
	window->platform_window = platform_create_window(os_get_platform_os(os), title, width, height);

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

void window_set_title(const LSWindow *window, String title) {
	LS_ASSERT(window);

	platform_window_set_title(window->platform_window, title);
}

void window_set_size(const LSWindow *window, int32 width, int32 height) {
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