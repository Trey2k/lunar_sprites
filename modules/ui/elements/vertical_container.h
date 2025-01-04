#ifndef UI_VERTICAL_CONTAINER_H
#define UI_VERTICAL_CONTAINER_H

#include "core/core.h"

#include "modules/ui/elements.h"
#include "modules/ui/theme.h"

typedef struct {
	uint32 spacing;
	Slice *children;
	Slice32 *child_bounds;
} UIVerticalContainer;

void ui_vertical_container_destroy(UIVerticalContainer *container);

void ui_vertical_container_draw(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);
void ui_vertical_container_calculate_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);

#endif // UI_VERTICAL_CONTAINER_H