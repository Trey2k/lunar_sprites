#ifndef CORE_H
#define CORE_H

/* --------------TYPES-------------- */
#include "core/types/hashtable/hashtable_int.h"
#include "core/types/hashtable/hashtable_string.h"
#include "core/types/slice.h"
#include "core/types/string.h"
#include "core/types/vector/vector2.h"
/* --------------------------------- */

/* --------------UTILS-------------- */
#include "core/debug.h"
#include "core/flags.h"
#include "core/log.h"
#include "core/memory.h"
/* --------------------------------- */

/* --------------MANAGERS-------------- */
#include "core/events/event_manager.h"
#include "core/input/input_manager.h"
/* ------------------------------------ */

/* --------------COMPONENTS-------------- */
#include "core/os/os.h"
#include "core/version.h"
#include "core/window.h"
/* -------------------------------------- */

void core_register_flags();

void core_init();
void core_start(int32 argc, char *argv[]);

void core_poll();
void core_deinit();

const InputManager *core_get_input_manager();

void core_set_active_window(const LSWindow *window);

void core_handle_press(LSKeycode keycode);
void core_handle_release(LSKeycode keycode);

void core_handle_mouse_press(LSMouseButton button, Vector2i position);
void core_handle_mouse_release(LSMouseButton button, Vector2i position);
void core_handle_mouse_move(Vector2i position);
void core_handle_mouse_enter(Vector2i position);
void core_handle_mouse_leave(Vector2i position);

#endif // CORE_H
