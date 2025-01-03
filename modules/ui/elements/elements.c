#include "elements.h"

void ui_draw_element(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			ui_draw_label(&element->label, outer_bounds, inner_bounds);
			break;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			break;
	}
}

void ui_element_destroy(UIElement *element) {
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			ui_label_destroy(&element->label);
			break;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			break;
	}
}

const UIElementTheme *ui_element_get_theme(const UIElement *element) {
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			return element->label.theme;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			return NULL;
	}
}

Vector2u ui_element_get_size(const UIElement *element) {
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			return element->label.size;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			return vec2u(0, 0);
	}
}

Vector2u ui_element_get_position(const UIElement *element) {
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			return element->label.position;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			return vec2u(0, 0);
	}
}