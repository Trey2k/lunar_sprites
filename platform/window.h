#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include "core/input/keycodes.h"
#include "core/math/vector.h"
#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#include "platform/os.h"

#include "renderer/window.h"

typedef struct PlatformWindow PlatformWindow;

PlatformWindow *platform_create_window(WindowConfig config, LSWindow *parent);
void platform_destroy_window(PlatformWindow *window);

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window);

void platform_window_poll(PlatformWindow *window);

void platform_window_set_title(PlatformWindow *window, String title);
void platform_window_set_min_size(PlatformWindow *window, Vector2u size);
Vector2u platform_window_get_min_size(const PlatformWindow *window);
void platform_window_set_max_size(PlatformWindow *window, Vector2u size);
Vector2u platform_window_get_max_size(const PlatformWindow *window);
void platform_window_set_size(PlatformWindow *window, Vector2u size);
Vector2u platform_window_get_size(const PlatformWindow *window);

void platform_window_set_fullscreen(PlatformWindow *window, bool fullscreen);
void platform_window_set_resizable(PlatformWindow *window, bool resizable);

void platform_window_show(PlatformWindow *window);
void platform_window_hide(PlatformWindow *window);

bool platform_window_is_visible(const PlatformWindow *window);
bool platform_window_is_fullscreen(const PlatformWindow *window);
bool platform_window_should_close(const PlatformWindow *window);

#endif // PLATFORM_WINDOW_H