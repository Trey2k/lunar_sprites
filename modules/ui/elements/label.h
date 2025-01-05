#ifndef UI_LABEL_IMPL_H
#define UI_LABEL_IMPL_H

#include "core/core.h"

#include "modules/ui/elements.h"
#include "modules/ui/theme.h"

typedef struct {
	char *text;
	uint32 padding;

	Slice *render_lines;
	Slice32 *render_lines_width;
	UITextWrapMode wrap_mode;

	Vector2u prev_inner_bounds;
	Vector2u prev_outer_bounds;

	UIElementTheme *theme;
} UILabel;

void ui_draw_label(UIElement *label);
void ui_label_destroy(UILabel *label);

void ui_label_calculate_size(UIElement *label, Vector2u outer_bounds, Vector2u inner_bounds);

void ui_label_handle_event(UIElement *label, Event *event);

#endif // UI_LABEL_IMPL_H