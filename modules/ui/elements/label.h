#ifndef UI_LABEL_IMPL_H
#define UI_LABEL_IMPL_H

#include "core/core.h"

#include "../theme.h"
#include "modules/ui/elements.h"

#define UI_LABEL_BUFFER_SIZE 1024

typedef struct {
	char *text;
	char buffer[UI_LABEL_BUFFER_SIZE];

	uint32 padding;

	Slice *render_lines;
	Vector2u prev_inner_bounds;
	Vector2u prev_outer_bounds;

	UIElementTheme *theme;
} UILabel;

void ui_draw_label(UIElement *label, Vector2u outer_bounds, Vector2u inner_bounds);
void ui_label_destroy(UILabel *label);

void ui_label_calculate_size(UIElement *label, Vector2u outer_bounds, Vector2u inner_bounds);

#endif // UI_LABEL_IMPL_H