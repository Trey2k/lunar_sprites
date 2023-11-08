#ifndef CORE_H
#define CORE_H

/* --------------TYPES-------------- */
#include "core/types/hashtable/hashtable_int.h"
#include "core/types/hashtable/hashtable_string.h"
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
#include "core/window.h"
/* -------------------------------------- */

void core_register_flags();

void core_init(int32 argc, char *argv[]);
void core_poll();
void core_deinit();

const InputManager *core_get_input_manager();

void core_handle_press(const LSWindow *window, LSKeycode keycode);
void core_handle_release(const LSWindow *window, LSKeycode keycode);

void core_handle_mouse_press(const LSWindow *window, LSMouseButton button, Vector2i position);
void core_handle_mouse_release(const LSWindow *window, LSMouseButton button, Vector2i position);
void core_handle_mouse_move(const LSWindow *window, Vector2i position);
void core_handle_mouse_enter(const LSWindow *window, Vector2i position);
void core_handle_mouse_leave(const LSWindow *window, Vector2i position);

#endif // CORE_H
