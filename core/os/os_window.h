#ifndef OS_WINDOW_H
#define OS_WINDOW_H

#include "core/os/os.h"
#include "core/string.h"
#include "core/typedefs.h"

#define MIN_WINDOW_WIDTH 50
#define MIN_WINDOW_HEIGHT 50

typedef struct LunarWindow LunarWindow;

LunarWindow *os_window_create(const OS *const os, const String title, int32 width, int32 height);
void window_destroy(LunarWindow *window);

void window_poll(const LunarWindow *window);

#endif // OS_WINDOW_H