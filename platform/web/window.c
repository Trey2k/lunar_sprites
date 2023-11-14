#include "platform/web/window.h"
#include "platform/os.h"
#include "platform/window.h"
#include "renderer/window.h"

#include "core/core.h"

PlatformWindow *platform_create_window(const PlatformOS *os, WindowConfig config) {
	PlatformWindow *window = ls_malloc(sizeof(PlatformWindow));

	window->title = config.title;
	window->width = config.size.x;
	window->height = config.size.y;
	window->hidden = config.hidden;
	window->fullscreen = false;

	return window;
}

void platform_destroy_window(PlatformWindow *window) {
	ls_free(window);
}

void platform_window_poll(const PlatformWindow *window) {
}

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window) {
	return 0;
}

void platform_window_set_fullscreen(PlatformWindow *window, bool fullscreen) {
	window->fullscreen = fullscreen;
}

void platform_window_show(PlatformWindow *window) {
	window->hidden = false;
}

void platform_window_hide(PlatformWindow *window) {
	window->hidden = true;
}

void platform_window_set_title(PlatformWindow *window, const char *title) {
	window->title = title;
}

void platform_window_set_size(PlatformWindow *window, int32 width, int32 height) {
	window->width = width;
	window->height = height;
}

bool platform_window_is_visible(const PlatformWindow *window) {
	return !window->hidden;
}

bool platform_window_is_fullscreen(const PlatformWindow *window) {
	return window->fullscreen;
}