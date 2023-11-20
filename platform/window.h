#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include "core/input/keycodes.h"
#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "core/types/vector/vector2.h"

#include "platform/os.h"

#include "renderer/window.h"

typedef struct PlatformWindow PlatformWindow;

PlatformWindow *platform_create_window(const PlatformOS *os, WindowConfig config, const Renderer *renderer);
void platform_destroy_window(PlatformWindow *window);

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window);

void platform_window_poll(PlatformWindow *window);

void platform_window_set_title(PlatformWindow *window, String title);
void platform_window_set_size(PlatformWindow *window, int32 width, int32 height);

void platform_window_set_fullscreen(PlatformWindow *window, bool fullscreen);

void platform_window_show(PlatformWindow *window);
void platform_window_hide(PlatformWindow *window);

bool platform_window_is_visible(const PlatformWindow *window);
bool platform_window_is_fullscreen(const PlatformWindow *window);

#endif // PLATFORM_WINDOW_H