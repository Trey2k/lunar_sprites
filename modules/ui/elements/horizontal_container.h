#ifndef UI_HORIZONTAL_CONTAINER_H
#define UI_HORIZONTAL_CONTAINER_H

#include "core/core.h"

#include "modules/ui/elements.h"
#include "modules/ui/theme.h"

typedef struct {
	uint32 spacing;
	Slice64 *children;
	UIAllignment alignment;
} UIHorizontalContainer;

void ui_horizontal_container_destroy(UIHorizontalContainer *container);
void ui_horizontal_container_draw(UIElement *element);
void ui_horizontal_container_calculate_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);

void ui_horizontal_container_handle_event(UIElement *element, Event *event);

#endif // UI_HORIZONTAL_CONTAINER_H