#ifndef INPUT_METHODS_H
#define INPUT_METHODS_H

#include "core/events/events.h"
#include "core/input/keycodes.h"
#include "core/types/vector2.h"

void input_poll();

void input_handle_press(const LSWindow *window, LSKeycode keycode);
void input_handle_release(const LSWindow *window, LSKeycode keycode);

bool input_is_key_pressed(LSKeycode keycode);
bool input_is_key_just_pressed(LSKeycode keycode);

bool input_is_key_released(LSKeycode keycode);
bool input_is_key_just_released(LSKeycode keycode);

void input_handle_mouse_press(const LSWindow *window, LSMouseButton button, Vector2i position);
void input_handle_mouse_release(const LSWindow *window, LSMouseButton button, Vector2i position);
void input_handle_mouse_move(const LSWindow *window, Vector2i position);
void input_handle_mouse_enter(const LSWindow *window, Vector2i position);
void input_handle_mouse_leave(const LSWindow *window, Vector2i position);

bool input_is_mouse_pressed(LSMouseButton button);
bool input_is_mouse_just_pressed(LSMouseButton button);

bool input_is_mouse_released(LSMouseButton button);
bool input_is_mouse_just_released(LSMouseButton button);

Vector2i input_get_mouse_position();

#endif // INPUT_METHODS_H