#ifndef WINDOW_H
#define WINDOW_H

#include "core/input/input_manager.h"
#include "core/os/os.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "core/types/vector/vector2.h"
#include "core/window.h"

#if defined(LINUXBSD_ENABLED)
typedef unsigned long LSNativeWindow;
#elif defined(WINDOWS_ENABLED)
typedef void *LSNativeWindow;
#elif defined(WEB_ENABLED)
typedef int32 LSNativeWindow;
#endif // LINUXBSD_ENABLED

typedef struct {
	Vector2i position;
	Vector2i size;
	String title;
	bool fullscreen;
	bool hidden;
} WindowConfig;

LSWindow *renderer_create_window(WindowConfig config);
void window_destroy(LSWindow *window);

LSNativeWindow window_get_native_window(const LSWindow *window);

void window_poll(const LSWindow *window);

void window_set_title(LSWindow *window, const char *title);
void window_set_size(LSWindow *window, int32 width, int32 height);

void window_make_current(const LSWindow *window);
void window_swap_buffers(const LSWindow *window);

void window_set_fullscreen(LSWindow *window, bool fullscreen);

void window_show(LSWindow *window);
void window_hide(LSWindow *window);

bool window_is_visible(const LSWindow *window);
bool window_is_fullscreen(const LSWindow *window);

#endif // WINDOW_H