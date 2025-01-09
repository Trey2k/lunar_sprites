#include "label.h"
#include "elements.h"
#include "modules/ui/elements.h"
#include "modules/ui/ui.h"

#include "core/types/string.h"
#include "core/types/typedefs.h"

#include "renderer/batch_renderer.h"

#include "modules/font/font.h"

UIElement *ui_label_create(const Font *font, String text, UITextWrapMode wrap_mode) {
	UIElement *element = (UIElement *)ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_LABEL;
	element->label.text = ls_str_copy(text);
	element->label.theme = (UIElementTheme *)ls_malloc(sizeof(UIElementTheme));
	*element->label.theme = (UIElementTheme){
		.background_color = COLOR_DARK_GREY,
		.border_color = COLOR_GREY,
		.radius = 10,
		.border_size = 2,
		.font_color = COLOR_WHITE,
		.font_size = 64,
		.font = NULL,
		.texture = NULL,
		.text_alignment = UI_TEXT_ALIGN_CENTER,
	};

	element->label.wrap_mode = wrap_mode;

	element->label.render_lines = slice_create(16, true);
	element->label.render_lines_width = slice32_create(16);

	element->label.theme->font = font;
	element->label.padding = 10;

	element->layout.mode = UI_LAYOUT_MODE_ANCHOR;
	element->layout.anchors = UI_ANCHOR_TOP | UI_ANCHOR_LEFT;
	element->min_size = vec2u(0, 0);
	element->max_size = vec2u(0, 0);

	return element;
}

void ui_label_destroy(UILabel *label) {
	ls_free(label->text);
	slice_destroy(label->render_lines);
}

void ui_label_set_theme(UIElement *element, const UIElementTheme *theme) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_LABEL);

	*element->label.theme = *theme;
}

static void label_draw_lines(UIElement *label_elm, String text) {
	UILabel *label = &label_elm->label;
	size_t n_lines = slice_get_size(label->render_lines);
	uint32 font_size = label->theme->font_size;
	uint32 total_text_height = n_lines * font_size;
	for (size_t i = 0; i < n_lines; i++) {
		Vector2 rendor_pos = label_elm->position;
		// center the text vertically
		rendor_pos.y += ((float32)label_elm->size.y / 2) - ((float32)total_text_height / 2);
		rendor_pos.y += i * label->theme->font_size;

		uint32 line_width = slice32_get(label->render_lines_width, i).u32;

		switch (label->theme->text_alignment) {
			case UI_TEXT_ALIGN_CENTER: {
				rendor_pos.x = label_elm->position.x + ((float32)label_elm->size.x / 2) - ((float32)line_width / 2);
			} break;
			case UI_TEXT_ALIGN_RIGHT: {
				rendor_pos.x = label_elm->position.x + label_elm->size.x - line_width;
			} break;
			case UI_TEXT_ALIGN_LEFT:
			default:
				break;
		};
		char *line = slice_get(label->render_lines, i).ptr;
		Vector2u line_size = font_draw_text(label->theme->font, font_size, label->theme->font_color, line, rendor_pos);
		rendor_pos.y += line_size.y;
	}
}

void ui_draw_label(UIElement *label_elm) {
	// Only draw the background if it has an alpha value
	UILabel *label = &label_elm->label;
	Vector2u label_size = label_elm->size;

	if (label->theme->background_color.a > 0) {
		if (label->theme->border_size > 0) {
			batch_renderer_draw_rect_outline(label->theme->texture, label->theme->background_color, label->theme->border_color, label->theme->radius, label_elm->position, label_size, label->theme->border_size);
		} else {
			batch_renderer_draw_rect(label->theme->texture, label->theme->background_color, label->theme->radius, label_elm->position, label_size);
		}
	}

	label_draw_lines(label_elm, label->text);
}

void ui_label_set_text(UIElement *element, String text) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_LABEL);

	ls_free(element->label.text);
	element->label.text = ls_str_copy(text);
}

static Vector2u ui_label_split_lines(UIElement *label_elm, Vector2u max_size) {
	UIElementTheme *theme = label_elm->label.theme;

	size_t text_len = ls_str_length(label_elm->label.text);
	Vector2u text_size = font_get_text_size(theme->font, theme->font_size, label_elm->label.text);

	uint32 used_y = 0;
	uint32 max_x = 0;

	char *l_text = label_elm->label.text;
	while (text_size.x > max_size.x) {
		bool failed = false;
		switch (label_elm->label.wrap_mode) {
			case UI_TEXT_WRAP_WORD: {
				// Find the last space before the max width
				for (size_t i = 0; i < text_len; i++) {
					if (l_text[text_len - i] == ' ') {
						l_text[text_len - i] = '\0';
						Vector2u line_size = font_get_text_size(theme->font, theme->font_size, l_text);
						l_text[text_len - i] = ' ';
						if (line_size.x < max_size.x) {
							char *new_line = ls_malloc(text_len - i);
							ls_str_copy_to(new_line, l_text, text_len - i);
							new_line[text_len - i] = '\0';
							slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
							slice32_append(label_elm->label.render_lines_width, SLICE_VAL32(u32, line_size.x));
							l_text += (text_len - i) + 1;
							text_len -= i;
							used_y += line_size.y;
							if (line_size.x > max_x) {
								max_x = line_size.x;
							}
							break;
						}
					}
				}
				failed = true;
			} break;
			case UI_TEXT_WRAP_CHAR: {
				for (size_t i = 0; i < text_len; i++) {
					if (l_text[text_len - i] == ' ') {
						char old_c = l_text[text_len - i];
						l_text[text_len - i] = '\0';
						Vector2u line_size = font_get_text_size(theme->font, theme->font_size, l_text);
						l_text[text_len - i] = old_c;
						if (line_size.x < max_size.x) {
							char *new_line = ls_malloc(text_len - i);
							ls_str_copy_to(new_line, l_text, text_len - i);
							new_line[text_len - i] = '\0';
							slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
							slice32_append(label_elm->label.render_lines_width, SLICE_VAL32(u32, line_size.x));
							l_text += (text_len - i);
							text_len -= i;
							used_y += line_size.y;
							if (line_size.x > max_x) {
								max_x = line_size.x;
							}
							break;
						}
					}
				}
				failed = true;
			} break;
			default: {
				failed = true;
			} break;
		};
		text_size = font_get_text_size(theme->font, theme->font_size, l_text);
		if (text_size.y + used_y > max_size.y) {
			break;
		}

		if (failed || text_size.x < max_size.x) {
			char *new_line = ls_str_copy(l_text);
			slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
			slice32_append(label_elm->label.render_lines_width, SLICE_VAL32(u32, text_size.x));
			used_y += text_size.y;
			if (text_size.x > max_x) {
				max_x = text_size.x;
			}
			// Exit the loop
			text_size = vec2u(0, 0);
		}
	}

	return vec2u(max_x, used_y);
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

	// Otherwise, recalculate the text size and render lines
	label_elm->label.prev_inner_bounds = inner_bounds;
	label_elm->label.prev_outer_bounds = outer_bounds;

	Vector2u max_size = vec2u_sub(outer_bounds, inner_bounds);

	if (label_elm->max_size.x > 0) {
		max_size.x = label_elm->max_size.x;
	}

	if (label_elm->max_size.y > 0) {
		max_size.y = label_elm->max_size.y;
	}

	if (max_size.x < label_elm->min_size.x) {
		max_size.x = label_elm->min_size.x;
	}

	if (max_size.y < label_elm->min_size.y) {
		max_size.y = label_elm->min_size.y;
	}

	slice_clear(label_elm->label.render_lines);
	slice32_clear(label_elm->label.render_lines_width);

	Vector2u text_size = font_get_text_size(theme->font, theme->font_size, label_elm->label.text);
	if (text_size.x > max_size.x && label_elm->label.wrap_mode != UI_TEXT_WRAP_NONE) {
		// Text does not fit on one line, split it up
		text_size = ui_label_split_lines(label_elm, max_size);
		label_elm->size = vec2u_add(text_size, vec2u(label_elm->label.padding, label_elm->label.padding));
	} else {
		// Text fits on one line
		char *new_line = ls_str_copy(label_elm->label.text);
		slice_append(label_elm->label.render_lines, SLICE_VAL(ptr, new_line));
		slice32_append(label_elm->label.render_lines_width, SLICE_VAL32(u32, text_size.x));
		label_elm->size = vec2u_add(text_size, vec2u(label_elm->label.padding, label_elm->label.padding));
	}

	// TODO: Add container layout settings
	if (label_elm->layout.mode == UI_LAYOUT_MODE_CONTAINER) {
		if (label_elm->layout.container_size.x > 0) {
			label_elm->size.x = label_elm->layout.container_size.x;
		}

		if (label_elm->layout.container_size.y > 0) {
			label_elm->size.y = label_elm->layout.container_size.y;
		}
	}

	if (label_elm->min_size.y > 0 && label_elm->size.y < label_elm->min_size.y) {
		label_elm->size.y = label_elm->min_size.y;
	}

	if (label_elm->min_size.y > 0 && label_elm->size.x < label_elm->min_size.x) {
		label_elm->size.x = label_elm->min_size.x;
	}
}

void ui_label_handle_event(UIElement *label_elm, Event *event) {
	LS_ASSERT(label_elm->type == UI_ELEMENT_TYPE_LABEL);
	if (event->type != EVENT_MOUSE || event->mouse.type != EVENT_MOUSE_RELEASED || event->mouse.button != LS_MOUSE_BUTTON_LEFT) {
		return;
	}

	ls_printf("Label clicked: %s\n", label_elm->label.text);
}

void ui_label_set_layout(UIElement *label_elm, UILayout layout) {
	LS_ASSERT(label_elm->type == UI_ELEMENT_TYPE_LABEL);
	label_elm->layout = layout;
	// Force a recalculation of the label
	label_elm->label.prev_inner_bounds = vec2u(0, 0);
}