#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "core/api.h"
#include "core/events/event_manager.h"
#include "core/input/keycodes.h"
#include "core/math/vector.h"
#include "core/types/typedefs.h"
#include "core/window.h"

typedef struct InputManager InputManager;

InputManager *ls_create_input_manager(const EventManager *event_manager);
void input_manager_destroy(InputManager *input_manager);

void input_poll(InputManager *input_manager);

void input_set_active_window(InputManager *input_manager, const LSWindow *window);

void input_handle_window_open(InputManager *input_manager, const LSWindow *window);
void input_handle_window_close(InputManager *input_manager);

void input_handle_press(InputManager *input_manager, LSKeycode keycode);
void input_handle_release(InputManager *input_manager, LSKeycode keycode);

void input_handle_mouse_press(InputManager *input_manager, LSMouseButton button, Vector2i position);
void input_handle_mouse_release(InputManager *input_manager, LSMouseButton button, Vector2i position);
void input_handle_mouse_move(InputManager *input_manager, Vector2i position);
void input_handle_mouse_enter(InputManager *input_manager, Vector2i position);
void input_handle_mouse_leave(InputManager *input_manager, Vector2i position);
void input_handle_resize(InputManager *input_manager, Vector2u size);

bool input_is_ready(const InputManager *input_manager);

LS_EXPORT bool input_is_key_pressed(const InputManager *input_manager, LSKeycode keycode);
LS_EXPORT bool input_is_key_just_pressed(const InputManager *input_manager, LSKeycode keycode);
LS_EXPORT bool input_is_key_released(const InputManager *input_manager, LSKeycode keycode);
LS_EXPORT bool input_is_key_just_released(const InputManager *input_manager, LSKeycode keycode);

LS_EXPORT bool input_is_mouse_pressed(const InputManager *input_manager, LSMouseButton button);
LS_EXPORT bool input_is_mouse_just_pressed(const InputManager *input_manager, LSMouseButton button);
LS_EXPORT bool input_is_mouse_released(const InputManager *input_manager, LSMouseButton button);
LS_EXPORT bool input_is_mouse_just_released(const InputManager *input_manager, LSMouseButton button);

Vector2i input_get_mouse_position(const InputManager *input_manager);

#endif // INPUT_MANAGER_H