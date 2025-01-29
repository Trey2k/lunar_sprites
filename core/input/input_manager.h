#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "core/api.h"
#include "core/input/keycodes.h"
#include "core/math/vector.h"
#include "core/types/typedefs.h"
#include "core/window.h"

void input_poll();

void input_set_active_window(const LSWindow *window);

void input_handle_press(LSKeycode keycode);
void input_handle_release(LSKeycode keycode);

void input_handle_mouse_press(LSMouseButton button, Vector2u position);
void input_handle_mouse_release(LSMouseButton button, Vector2u position);
void input_handle_mouse_move(Vector2u position);
void input_handle_mouse_enter(Vector2u position);
void input_handle_mouse_leave(Vector2u position);

bool input_is_ready();

LS_EXPORT bool input_is_key_pressed(const LSKeycode keycode);
LS_EXPORT bool input_is_key_just_pressed(const LSKeycode keycode);
LS_EXPORT bool input_is_key_released(const LSKeycode keycode);
LS_EXPORT bool input_is_key_just_released(const LSKeycode keycode);

LS_EXPORT bool input_is_mouse_pressed(const LSMouseButton button);
LS_EXPORT bool input_is_mouse_just_pressed(const LSMouseButton button);
LS_EXPORT bool input_is_mouse_released(const LSMouseButton button);
LS_EXPORT bool input_is_mouse_just_released(const LSMouseButton button);

LS_EXPORT Vector2u input_get_mouse_position();

#endif // INPUT_MANAGER_H