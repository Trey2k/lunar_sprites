#ifndef OS_WINDOW_H
#define OS_WINDOW_H

#include "core/os/os.h"
#include "core/string.h"
#include "core/typedefs.h"

#define MIN_WINDOW_WIDTH 50
#define MIN_WINDOW_HEIGHT 50

typedef struct LSWindow LSWindow;

LSWindow *os_window_create(const OS *os, const String title, int32 width, int32 height);
void window_destroy(LSWindow *window);

void window_poll(const LSWindow *window);

#endif // OS_WINDOW_H