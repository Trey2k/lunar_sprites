#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "core/core.h"

#include "renderer/renderer.h"

void ls_main_loop_init(LSCore *core, LSWindow *root_window);
void ls_main_loop_deinit();

void ls_main_loop();

#endif // MAIN_LOOP_H