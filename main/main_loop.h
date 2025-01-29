#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "core/core.h"

#include "renderer/renderer.h"

void ls_main_loop_init(LSWindow *root_window);

// Draws a frame.
void ls_draw_frame();
// Polls input and draws a frame.
void ls_main_loop();

#endif // MAIN_LOOP_H