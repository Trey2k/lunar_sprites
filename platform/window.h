#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include "core/input/keycodes.h"
#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "core/types/vector/vector2.h"

#include "platform/os.h"

typedef struct PlatformWindow PlatformWindow;

#if defined(LINUXBSD_ENABLED)
typedef unsigned long LSNativeWindow;
#elif defined(WINDOWS_ENABLED)
typedef void *LSNativeWindow;
#endif // LINUXBSD_ENABLED

PlatformWindow *platform_create_window(const PlatformOS *os, String title, int32 width, int32 height);
void platform_destroy_window(PlatformWindow *window);

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window);

void platform_window_poll(const PlatformWindow *window);

void platform_window_set_title(const PlatformWindow *window, String title);
void platform_window_set_size(const PlatformWindow *window, int32 width, int32 height);

#endif // PLATFORM_WINDOW_H