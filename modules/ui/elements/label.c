#include "label.h"

#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "elements.h"
#include "modules/font/font.h"
#include "modules/ui/elements.h"
#include "modules/ui/ui.h"

static const UIElementTheme DEFAULT_LABEL_THEME = {
	.background_color = COLOR_DARK_GREY,
	.border_color = COLOR_GREY,
	.radius = 10,
	.border_size = 2,
	.font_color = COLOR_WHITE,
	.font_size = 64,
	.font = NULL,
	.texture = NULL,
};

UIElement *ui_label_create(const Font *font, String text, Vector2u position) {
	UIElement *element = ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_LABEL;
	element->label.text = ls_str_copy(text);
	element->label.theme = ls_malloc(sizeof(UIElementTheme));
	*element->label.theme = DEFAULT_LABEL_THEME;

	element->label.anchors = UI_ANCHOR_TOP | UI_ANCHOR_RIGHT;

	element->label.min_size = vec2u(0, 0);

	element->label.theme->font = font;

	element->label.position = position;

	element->label.padding = 10;

	return element;
}

UIElementTheme *ui_label_get_theme(UIElement *element) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_LABEL);
	return element->label.theme;
}

void ui_label_destroy(UILabel *label) {
	ls_free(label->text);
}

static Vector2u label_get_used_sapce(UILabel *label, String text, Vector2u outer_bounds, Vector2u inner_bounds) {
	ls_str_copy_to(label->buffer, text, UI_LABEL_BUFFER_SIZE);
	size_t text_len = ls_str_length(label->buffer);
	Vector2u total_space_used = vec2u(0, 0);

	// See if the text fits on one line
	Vector2u text_size = ui_get_text_size(label->theme, text);
	if (text_size.x > outer_bounds.x) {
		// Text does not fit on one line, split it up
		while (text_size.x > outer_bounds.x) {
			label->buffer[text_len - 1] = '\0';
			text_len--;
			text_size = ui_get_text_size(label->theme, label->buffer);
		}

		// Draw the first part of the text
		total_space_used = ui_get_text_size(label->theme, label->buffer);

		// If we can, draw the rest of the text
		Vector2u next_line_size = ui_get_text_size(label->theme, &text[text_len]);
		Vector2u new_inner_bounds = vec2u(inner_bounds.x, inner_bounds.y + text_size.y);

		if (next_line_size.y <= new_inner_bounds.y) {
			Vector2u space_ued = label_get_used_sapce(label, &text[text_len], outer_bounds, new_inner_bounds);
			total_space_used.y += space_ued.y;
		}
	} else {
		// Text fits on one line, draw it
		total_space_used = ui_get_text_size(label->theme, label->buffer);
	}

	return total_space_used;
}

static Vector2u label_draw_lines(UILabel *label, String text, Vector2u outer_bounds, Vector2u inner_bounds, Vector2u text_position) {
	// calculate position relative to ouuter bounds

	ls_str_copy_to(label->buffer, text, UI_LABEL_BUFFER_SIZE);
	size_t text_len = ls_str_length(label->buffer);
	Vector2u total_space_used = vec2u(0, 0);

	// See if the text fits on one line
	Vector2u text_size = ui_get_text_size(label->theme, text);
	if (text_size.x > outer_bounds.x) {
		// Text does not fit on one line, split it up
		while (text_size.x > outer_bounds.x) {
			label->buffer[text_len - 1] = '\0';
			text_len--;
			text_size = ui_get_text_size(label->theme, label->buffer);
		}

		// Draw the first part of the text
		total_space_used = ui_draw_text(label->theme, label->buffer, text_position);

		// If we can, draw the rest of the text
		Vector2u next_line_size = ui_get_text_size(label->theme, &text[text_len]);
		Vector2u new_inner_bounds = vec2u(inner_bounds.x, inner_bounds.y + text_size.y);

		if (next_line_size.y <= new_inner_bounds.y) {
			Vector2u space_ued = label_draw_lines(label, &text[text_len], outer_bounds, new_inner_bounds, text_position);
			total_space_used.y += space_ued.y;
		}
	} else {
		// Text fits on one line, draw it
		total_space_used = ui_draw_text(label->theme, label->buffer, text_position);
	}

	return total_space_used;
}

// Returns draw position, and updates the minimum size of the label
static void label_calculate_position(UILabel *label, Vector2u outer_bounds, Vector2u inner_bounds) {
	Vector2u l_size = label_get_used_sapce(label, label->text, outer_bounds, inner_bounds);

	if (label->anchors & UI_ANCHOR_TOP && label->anchors & UI_ANCHOR_BOTTOM) {
		label->position.y = inner_bounds.y;
		label->min_size.y = outer_bounds.y;
	} else if (label->anchors & UI_ANCHOR_TOP) {
		label->position.y = inner_bounds.y;
	} else if (label->anchors & UI_ANCHOR_BOTTOM) {
		label->position.y = outer_bounds.y - (l_size.y + label->padding);
	}

	if (label->anchors & UI_ANCHOR_LEFT && label->anchors & UI_ANCHOR_RIGHT) {
		label->position.x = inner_bounds.x;
		label->min_size.x = outer_bounds.x;
	} else if (label->anchors & UI_ANCHOR_LEFT) {
		label->position.x = inner_bounds.x;
	} else if (label->anchors & UI_ANCHOR_RIGHT) {
		label->position.x = outer_bounds.x - (l_size.x + label->padding);
	}

	if (label->anchors & UI_ANCHOR_CENTER) {
		// Center the label on the X axis if it is not anchored to the left or right
		if (!(label->anchors & UI_ANCHOR_LEFT || label->anchors & UI_ANCHOR_RIGHT)) {
			label->position.x = ((outer_bounds.x + inner_bounds.x) / 2) - (l_size.x / 2);
		}

		// Center the label on the Y axis if it is not anchored to the top or bottom
		if (!(label->anchors & UI_ANCHOR_TOP || label->anchors & UI_ANCHOR_BOTTOM)) {
			label->position.y = ((outer_bounds.y + inner_bounds.y) / 2) - (l_size.y / 2);
		}
	}
}

void ui_draw_label(UILabel *label, Vector2u outer_bounds, Vector2u inner_bounds) {
	// Only draw the background if it has an alpha value
	label_calculate_position(label, outer_bounds, inner_bounds);
	Vector2u text_position = label->position;

	if (label->theme->background_color.a > 0) {
		Vector2u used_space = label_get_used_sapce(label, label->text, outer_bounds, inner_bounds);
		// Add some padding to the used space

		Vector2u outline_space = vec2u_add(used_space, vec2u(label->padding, label->padding));

		if (label->min_size.x > used_space.x) {
			outline_space.x = label->min_size.x;
		}

		if (label->min_size.y > used_space.y) {
			outline_space.y = label->min_size.y;
		}

		text_position = vec2u(label->position.x + (label->padding / 2), label->position.y + (label->padding / 2));

		Vector2u bg_space = vec2u_sub(outline_space,
				vec2u(label->theme->border_size * 2, label->theme->border_size * 2));

		if (label->theme->border_size > 0) {
			ui_draw_rect(label->theme->texture, label->theme->border_color, label->theme->radius, label->position, outline_space);
			Vector2u bg_position = vec2u(label->position.x + label->theme->border_size, label->position.y + label->theme->border_size);
			ui_draw_rect(label->theme->texture, label->theme->background_color, label->theme->radius, bg_position, bg_space);
		} else {
			ui_draw_rect(label->theme->texture, label->theme->background_color, label->theme->radius, label->position, outline_space);
		}
	}

	label_draw_lines(label, label->text, outer_bounds, inner_bounds, text_position);
}

void ui_label_set_text(UIElement *element, String text) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_LABEL);

	ls_free(element->label.text);
	element->label.text = ls_str_copy(text);
}