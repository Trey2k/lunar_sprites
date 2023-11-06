#ifndef WINDOW_H
#define WINDOW_H

#include "core/os/os.h"
#include "core/typedefs.h"
#include "core/types/string.h"

#define MIN_WINDOW_WIDTH 50
#define MIN_WINDOW_HEIGHT 50

typedef struct LSWindow LSWindow;

LSWindow *window_create(String title, int32 width, int32 height);
void window_destroy(LSWindow *window);

unsigned long window_get_native_window(const LSWindow *window);

void window_poll(const LSWindow *window);

void window_set_title(const LSWindow *window, String title);
void window_set_size(const LSWindow *window, int32 width, int32 height);

#endif // WINDOW_H