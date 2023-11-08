#ifndef WINDOW_H
#define WINDOW_H

#include "core/input/input_manager.h"
#include "core/os/os.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#include "renderer/renderer.h"

LSWindow *ls_create_window(const Renderer *renderer, const char *title, int32 width, int32 height);
void window_destroy(LSWindow *window);

unsigned long window_get_native_window(const LSWindow *window);

void window_poll(const LSWindow *window);

void window_set_title(const LSWindow *window, const char *title);
void window_set_size(const LSWindow *window, int32 width, int32 height);

void window_make_current(const LSWindow *window);
void window_swap_buffers(const LSWindow *window);

#endif // WINDOW_H