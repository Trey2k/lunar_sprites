#ifndef WINDOW_H
#define WINDOW_H

#include "core/core.h"

#include "renderer/renderer.h"

#if defined(LINUXBSD_ENABLED)
typedef unsigned long LSNativeWindow;
#elif defined(WINDOWS_ENABLED)
typedef void *LSNativeWindow;
#elif defined(WEB_ENABLED)
typedef int LSNativeWindow;
#endif // LINUXBSD_ENABLED

typedef struct {
	Vector2i position;
	Vector2i size;

	String title;

	bool fullscreen;
	bool hidden;

	bool root_window;
} WindowConfig;

LS_EXPORT LSWindow *renderer_create_window(const Renderer *renderer, WindowConfig config);
LS_EXPORT void window_destroy(LSWindow *window);

LS_EXPORT LSNativeWindow window_get_native_window(const LSWindow *window);

LS_EXPORT void window_poll(const LSWindow *window);

LS_EXPORT void window_set_title(LSWindow *window, const char *title);
LS_EXPORT void window_set_size(LSWindow *window, int32 width, int32 height);

LS_EXPORT void window_make_current(const LSWindow *window);
LS_EXPORT void window_swap_buffers(const LSWindow *window);

LS_EXPORT void window_set_fullscreen(LSWindow *window, bool fullscreen);

LS_EXPORT void window_show(LSWindow *window);
LS_EXPORT void window_hide(LSWindow *window);

LS_EXPORT bool window_is_visible(const LSWindow *window);
LS_EXPORT bool window_is_fullscreen(const LSWindow *window);

#endif // WINDOW_H