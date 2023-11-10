#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H

#include "core/input/keycodes.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "core/types/vector/vector2.h"

#include "platform/os.h"

typedef struct PlatformWindow PlatformWindow;

typedef enum {
	PLATFORM_INPUT_NONE,
	PLATFORM_INPUT_KEYPRESS,
	PLATFORM_INPUT_KEYRELEASE,
	PLATFORM_INPUT_MOUSEPRESS,
	PLATFORM_INPUT_MOUSERELEASE,
	PLATFORM_INPUT_MOUSEMOVE,
	PLATFORM_INPUT_MOUSEENTER,
	PLATFORM_INPUT_MOUSELEAVE,
} PlatformInputType;

typedef struct {
	PlatformInputType type;
	union {
		LSKeycode keycode;
		LSMouseButton button;
	};
	Vector2i position;
} PlatformInput;

#if defined(LINUXBSD_ENABLED)
typedef unsigned long LSNativeWindow;
#elif defined(WINDOWS_ENABLED)
typedef void *LSNativeWindow;
#endif // LINUXBSD_ENABLED

PlatformWindow *platform_create_window(const PlatformOS *os, String title, int32 width, int32 height);
void platform_destroy_window(PlatformWindow *window);

LSNativeWindow platform_window_get_native_window(const PlatformWindow *window);

PlatformInput *platform_window_poll(const PlatformWindow *window);

void platform_window_set_title(const PlatformWindow *window, String title);
void platform_window_set_size(const PlatformWindow *window, int32 width, int32 height);

#endif // PLATFORM_WINDOW_H