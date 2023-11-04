#include "core/input/input.h"

struct Input {
	const EventManager *event_manager;
};

Input *input_create(const EventManager *event_manager) {
	CORE_ASSERT(event_manager);

	Input *input = core_malloc(sizeof(Input));
	input->event_manager = event_manager;

	return input;
}