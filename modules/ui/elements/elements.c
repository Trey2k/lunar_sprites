#include "elements.h"

void ui_draw_element(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	// Updated element size and position
	ui_element_calculate_position(element, outer_bounds, inner_bounds);
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			ui_draw_label(element, outer_bounds, inner_bounds);
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

void ui_element_set_anchor(UIElement *element, uint32 anchors) {
	element->anchors = anchors;
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
	return element->size;
}

Vector2u ui_element_get_position(const UIElement *element) {
	return element->position;
}

void ui_element_set_min_size(UIElement *element, Vector2u min_size) {
	element->min_size = min_size;
}

Vector2u ui_element_get_min_size(const UIElement *element) {
	return element->min_size;
}

void ui_element_set_max_size(UIElement *element, Vector2u max_size) {
	element->max_size = max_size;
}

Vector2u ui_element_get_max_size(const UIElement *element) {
	return element->max_size;
}

void ui_element_calculate_position(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	// Start by calculating the size of the element.
	// The element at this point will set its size to be the minimum size based ont the bounds.
	// It is acceptable to have a size of 0,0 at this point.
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL: {
			ui_label_calculate_size(element, outer_bounds, inner_bounds);
		} break;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			return;
	}

	// Calculate the position and size of the element based on the anchor points.
	if (element->anchors & UI_ANCHOR_TOP && element->anchors & UI_ANCHOR_BOTTOM) {
		element->position.y = inner_bounds.y;
		element->size.y = outer_bounds.y - inner_bounds.y;
		if (element->min_size.y > 0 && element->size.y < element->min_size.y) {
			element->size.y = element->min_size.y;
		}

		if (element->max_size.y > 0 && element->size.y > element->max_size.y) {
			element->size.y = element->max_size.y;
		}
	} else if (element->anchors & UI_ANCHOR_TOP) {
		element->position.y = inner_bounds.y;
	} else if (element->anchors & UI_ANCHOR_BOTTOM) {
		element->position.y = outer_bounds.y - element->size.y;
	}

	if (element->anchors & UI_ANCHOR_LEFT && element->anchors & UI_ANCHOR_RIGHT) {
		element->position.x = inner_bounds.x;
		element->size.x = outer_bounds.x - inner_bounds.x;
		if (element->min_size.x > 0 && element->size.x < element->min_size.x) {
			element->size.x = element->min_size.x;
		}

		if (element->max_size.x > 0 && element->size.x > element->max_size.x) {
			element->size.x = element->max_size.x;
		}
	} else if (element->anchors & UI_ANCHOR_LEFT) {
		element->position.x = inner_bounds.x;
	} else if (element->anchors & UI_ANCHOR_RIGHT) {
		element->position.x = outer_bounds.x - element->size.x;
	}

	if (element->anchors & UI_ANCHOR_CENTER) {
		if (!(element->anchors & UI_ANCHOR_LEFT) && !(element->anchors & UI_ANCHOR_RIGHT)) {
			element->position.x = ((outer_bounds.x + inner_bounds.x) / 2) - (element->size.x / 2);
		}

		if (!(element->anchors & UI_ANCHOR_TOP) && !(element->anchors & UI_ANCHOR_BOTTOM)) {
			element->position.y = ((outer_bounds.y + inner_bounds.y) / 2) - (element->size.y / 2);
		}
	}
}