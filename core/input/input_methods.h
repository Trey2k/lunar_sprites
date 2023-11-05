#ifndef INPUT_METHODS_H
#define INPUT_METHODS_H

#include "core/event/event_manager.h"
#include "core/input/input.h"

Input *input_create(const EventManager *event_manager);
void input_destroy(Input *input);

void input_poll(Input *input);

void input_handle_press(Input *input, const LSWindow *window, LSKeycode keycode);
void input_handle_release(Input *input, const LSWindow *window, LSKeycode keycode);

bool input_is_key_pressed(const Input *input, LSKeycode keycode);
bool input_is_key_just_pressed(const Input *input, LSKeycode keycode);

bool input_is_key_released(const Input *input, LSKeycode keycode);
bool input_is_key_just_released(const Input *input, LSKeycode keycode);

#endif // INPUT_METHODS_H