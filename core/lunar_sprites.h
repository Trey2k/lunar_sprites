#ifndef CORE_H
#define CORE_H

/* --------------TYPES-------------- */
#include "core/types/hashtable/hashtable_int.h"
#include "core/types/hashtable/hashtable_string.h"
#include "core/types/string/string.h"
#include "core/types/vector/vector2.h"
/* --------------------------------- */

/* --------------UTILS-------------- */
#include "core/debug.h"
#include "core/log/log.h"
#include "core/memory/memory.h"
/* --------------------------------- */

/* --------------MANAGERS-------------- */
#include "core/events/event_manager.h"
#include "core/input/input_manager.h"
/* ------------------------------------ */

/* --------------COMPONENTS-------------- */
#include "core/os/os.h"
#include "core/window/window.h"
#include "renderer/renderer.h"
/* -------------------------------------- */

void ls_init(RendererBackend renderer_backend);
void ls_poll();
void ls_deinit();

void core_fatal(String message, ...);

const InputManager *ls_get_input_manager();

const LSWindow *ls_get_root_window();

void ls_handle_press(const LSWindow *window, LSKeycode keycode);
void ls_handle_release(const LSWindow *window, LSKeycode keycode);

void ls_handle_mouse_press(const LSWindow *window, LSMouseButton button, Vector2i position);
void ls_handle_mouse_release(const LSWindow *window, LSMouseButton button, Vector2i position);
void ls_handle_mouse_move(const LSWindow *window, Vector2i position);
void ls_handle_mouse_enter(const LSWindow *window, Vector2i position);
void ls_handle_mouse_leave(const LSWindow *window, Vector2i position);

#endif // CORE_H
