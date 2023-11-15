#ifndef WEBGL_H
#define WEBGL_H

#include "platform/web/window.h"

void webgl_init_canvas(PlatformWindow *window);
void webgl_deinit_canvas(PlatformWindow *window);

void webgl_make_current(const PlatformWindow *window);
void webgl_swap_buffers(const PlatformWindow *window);

#endif // WEBGL_H