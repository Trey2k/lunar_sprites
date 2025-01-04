#include "label.h"

#include "core/types/string.h"
#include "core/types/typedefs.h"
#include "elements.h"
#include "modules/font/font.h"
#include "modules/ui/elements.h"
#include "modules/ui/ui.h"

static UIElementTheme DEFAULT_LABEL_THEME = {
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
	element->label.render_lines = slice_create(16, true);
	element->label.theme->font = font;
	element->label.padding = 10;

	element->anchors = UI_ANCHOR_TOP | UI_ANCHOR_BOTTOM | UI_ANCHOR_LEFT;

	return element;
}

void ui_label_destroy(UILabel *label) {
	ls_free(label->text);
	slice_destroy(label->render_lines);
}

//TODO: Try to simplify this function
void ui_label_calculate_size(UIElement *label_elm, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(label_elm->type == UI_ELEMENT_TYPE_LABEL);

	UIElementTheme *theme = label_elm->label.theme;

	// Check if the text has been cached, and is still relevant
	if (vec2u_equals(label_elm->label.prev_inner_bounds, inner_bounds) &&
			vec2u_equals(label_elm->label.prev_outer_bounds, outer_bounds)) {
		return;
	}
	label_elm->label.prev_inner_bounds = inner_bounds;
	label_elm->label.prev_outer_bounds = outer_bounds;

	Vector2u max_size = vec2u_sub(outer_bounds, inner_bounds);
	// Otherwise, recalculate the text size and render lines
	size_t text_len = ls_str_length(label_elm->label.text);
	Vector2u text_size = ui_get_text_size(theme, label_elm->label.text);
	slice_clear(label_elm->label.render_lines);

	if (text_size.x <= max_size.x) {
		// Text fits on one line
		char *new_line = ls_str_copy(label_elm->label.text);
		slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
		label_elm->size = vec2u_add(text_size, vec2u(label_elm->label.padding, label_elm->label.padding));
		return;
	}

	// Text does not fit on one line, split it up
	char *l_text = label_elm->label.text;
	uint32 used_y = 0;
	uint32 max_x = 0;
	while (text_size.x > max_size.x) {
		// Try to split the text at a space character first
		bool split_on_space = false;
		for (size_t i = 0; i < text_len; i++) {
			if (l_text[text_len - i] == ' ') {
				l_text[text_len - i] = '\0';
				text_size = ui_get_text_size(theme, l_text);
				l_text[text_len - i] = ' ';
				if (text_size.x <= max_size.x) {
					// The line now fits
					char *new_line = ls_malloc(text_len - i);
					ls_str_copy_to(new_line, l_text, text_len - i);
					new_line[text_len - i] = '\0';
					slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
					// Add 1 to skip the space character
					l_text = &l_text[(text_len - i) + 1];
					used_y += text_size.y;
					max_x = max_x < text_size.x ? text_size.x : max_x;
					split_on_space = true;
					break;
				}
			}
		}

		// We failed to split on a space, so split on the last character that fits
		if (!split_on_space) {
			for (size_t i = 0; i < text_len; i++) {
				char old_c = l_text[text_len - i];
				l_text[text_len - i] = '\0';
				text_size = ui_get_text_size(theme, l_text);
				l_text[text_len - i] = old_c;
				if (text_size.x <= max_size.x) {
					// The line now fits
					char *new_line = ls_malloc(text_len - i);
					new_line[text_len - i] = '\0';
					ls_str_copy_to(new_line, l_text, text_len - i);
					slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
					l_text = &l_text[text_len - i];
					used_y += text_size.y;
					max_x = max_x < text_size.x ? text_size.x : max_x;
					break;
				}
			}
		}

		text_size = ui_get_text_size(theme, l_text);
		if (text_size.y + used_y > max_size.y) {
			break;
		}
	}

	if (text_size.y + used_y <= max_size.y) {
		// The remaining text fits on one line
		char *new_line = ls_str_copy(l_text);
		slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
		used_y += text_size.y;
		max_x = max_x < text_size.x ? text_size.x : max_x;
	}

	label_elm->size = vec2u(max_x + label_elm->label.padding, used_y + label_elm->label.padding);
}

static void label_draw_lines(UIElement *label_elm, String text, Vector2u outer_bounds, Vector2u inner_bounds, Vector2u text_position) {
	UILabel *label = &label_elm->label;
	size_t n_lines = slice_get_size(label->render_lines);
	Vector2u rendor_pos = text_position;
	for (size_t i = 0; i < n_lines; i++) {
		char *line = slice_get(label->render_lines, i).ptr;
		Vector2u line_size = ui_draw_text(label->theme, line, rendor_pos);
		rendor_pos.y += line_size.y;
	}
}

void ui_draw_label(UIElement *label_elm, Vector2u outer_bounds, Vector2u inner_bounds) {
	// Only draw the background if it has an alpha value
	Vector2u text_position = label_elm->position;

	UILabel *label = &label_elm->label;
	Vector2u label_size = label_elm->size;

	if (label->theme->background_color.a > 0) {
		// Add some padding to the used space

		text_position = vec2u(label_elm->position.x + (label->padding / 2), label_elm->position.y + (label->padding / 2));

		Vector2u bg_space = vec2u_sub(label_size,
				vec2u(label->theme->border_size * 2, label->theme->border_size * 2));

		if (label->theme->border_size > 0) {
			ui_draw_rect(label->theme->texture, label->theme->border_color, label->theme->radius, label_elm->position, label_size);
			Vector2u bg_position = vec2u(label_elm->position.x + label->theme->border_size, label_elm->position.y + label->theme->border_size);
			ui_draw_rect(label->theme->texture, label->theme->background_color, label->theme->radius, bg_position, bg_space);
		} else {
			ui_draw_rect(label->theme->texture, label->theme->background_color, label->theme->radius, label_elm->position, label_size);
		}
	}

	label_draw_lines(label_elm, label->text, outer_bounds, inner_bounds, text_position);
}

void ui_label_set_text(UIElement *element, String text) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_LABEL);

	ls_free(element->label.text);
	element->label.text = ls_str_copy(text);
}