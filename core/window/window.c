#include "core/window/window.h"
#include "core/debug.h"
#include "core/memory/memory.h"

#include "platform/window.h"

#include "renderer/context.h"

#include "core/lunar_sprites.h"

struct LSWindow {
	PlatformWindow *platform_window;

	Context *context;
};

LSWindow *ls_create_window(String title, int32 width, int32 height) {
	const OS *os = ls_get_os();
	LS_ASSERT(os);
	const Renderer *renderer = ls_get_renderer();
	LS_ASSERT(renderer);

	LSWindow *window = ls_malloc(sizeof(LSWindow));
	window->platform_window = platform_create_window(os_get_platform_os(os), title, width, height);

	window->context = renderer_context_create(renderer, window);
	renderer_context_make_current(window->context);
	renderer_context_swap_buffers(window->context);

	return window;
}

void window_destroy(LSWindow *window) {
	LS_ASSERT(window);

	platform_destroy_window(window->platform_window);
	ls_free(window);
}

unsigned long window_get_native_window(const LSWindow *window) {
	LS_ASSERT(window);

	return platform_window_get_native_window(window->platform_window);
}

void window_poll(const LSWindow *window) {
	LS_ASSERT(window);

	PlatformInput *input = platform_window_poll(window->platform_window);
	if (!input) {
		return;
	}

	PlatformInput *start = input;

	while (input->type != PLATFORM_INPUT_NONE) {
		switch (input->type) {
			case PLATFORM_INPUT_NONE: {
			} break;
			case PLATFORM_INPUT_KEYPRESS: {
				ls_handle_press(window, input->keycode);
			} break;

			case PLATFORM_INPUT_KEYRELEASE: {
				ls_handle_release(window, input->keycode);
			} break;

			case PLATFORM_INPUT_MOUSEPRESS: {
				ls_handle_mouse_press(window, input->button, input->position);
			} break;

			case PLATFORM_INPUT_MOUSERELEASE: {
				ls_handle_mouse_release(window, input->button, input->position);
			} break;

			case PLATFORM_INPUT_MOUSEMOVE: {
				ls_handle_mouse_move(window, input->position);
			} break;

			case PLATFORM_INPUT_MOUSEENTER: {
				ls_handle_mouse_enter(window, input->position);
			} break;

			case PLATFORM_INPUT_MOUSELEAVE: {
				ls_handle_mouse_leave(window, input->position);
			} break;

			default: {
				ls_log(LOG_LEVEL_WARNING, "Unknown platform input type: %d\n", input->type);
			} break;
		}

		input++;
	}

	// We are responsible for freeing the input
	ls_free(start);
}

void window_set_title(const LSWindow *window, String title) {
	LS_ASSERT(window);

	platform_window_set_title(window->platform_window, title);
}

void window_set_size(const LSWindow *window, int32 width, int32 height) {
	LS_ASSERT(window);

	platform_window_set_size(window->platform_window, width, height);
}