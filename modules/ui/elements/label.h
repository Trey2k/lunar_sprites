#ifndef UI_LABEL_IMPL_H
#define UI_LABEL_IMPL_H

#include "core/core.h"

#include "../theme.h"

#define UI_LABEL_BUFFER_SIZE 1024

typedef struct {
	char *text;
	char buffer[UI_LABEL_BUFFER_SIZE];
	UIElementTheme *theme;

	Vector2u position;
	Vector2u min_size;
	uint32 padding;
	uint32 anchors;
	// Is calculated when the label is drawn
	Vector2u size;
} UILabel;

void ui_draw_label(UILabel *label, Vector2u outer_bounds, Vector2u inner_bounds);
void ui_label_destroy(UILabel *label);

#endif // UI_LABEL_IMPL_H