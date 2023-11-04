#ifndef INPUT_H
#define INPUT_H

#include "core/core.h"
#include "core/event/event_manager.h"
#include "core/input/keycodes.h"

typedef struct Input Input;

Input *input_create(const EventManager *event_manager);
void input_destroy(Input *input);

#endif // INPUT_H