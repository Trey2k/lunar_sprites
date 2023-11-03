#ifndef OS_WINDOW_H
#define OS_WINDOW_H

#include "core/os/os.h"
#include "core/typedefs.h"

#define MIN_WINDOW_WIDTH 50
#define MIN_WINDOW_HEIGHT 50

typedef struct Window Window;

Window *os_window_create(const OS *os, String title, int32 width, int32 height);
void os_window_destroy(const OS *os, Window *window);

#endif // OS_WINDOW_H