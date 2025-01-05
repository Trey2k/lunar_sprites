#include "elements.h"

void ui_draw_element(UIElement *element) {
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL:
			ui_draw_label(element);
			break;
		case UI_ELEMENT_TYPE_VERTICAL_CONTAINER:
			ui_vertical_container_draw(element);
			break;
		case UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER:
			ui_horizontal_container_draw(element);
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
		case UI_ELEMENT_TYPE_VERTICAL_CONTAINER:
			ui_vertical_container_destroy(&element->vertical_container);
			break;
		case UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER:
			ui_horizontal_container_destroy(&element->horizontal_container);
			break;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			break;
	}
}

void ui_element_set_layout(UIElement *element, UILayout layout) {
	element->layout = layout;
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

static void ui_element_calculate_anchors(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	UILayout layout = element->layout;
	if (layout.anchors & UI_ANCHOR_TOP && layout.anchors & UI_ANCHOR_BOTTOM) {
		element->position.y = inner_bounds.y;
		element->size.y = outer_bounds.y - inner_bounds.y;
		if (element->min_size.y > 0 && element->size.y < element->min_size.y) {
			element->size.y = element->min_size.y;
		}

		if (element->max_size.y > 0 && element->size.y > element->max_size.y) {
			element->size.y = element->max_size.y;
		}
	} else if (layout.anchors & UI_ANCHOR_TOP) {
		element->position.y = inner_bounds.y;
	} else if (layout.anchors & UI_ANCHOR_BOTTOM) {
		element->position.y = outer_bounds.y - element->size.y;
	}

	if (layout.anchors & UI_ANCHOR_LEFT && layout.anchors & UI_ANCHOR_RIGHT) {
		element->position.x = inner_bounds.x;
		element->size.x = outer_bounds.x - inner_bounds.x;
		if (element->min_size.x > 0 && element->size.x < element->min_size.x) {
			element->size.x = element->min_size.x;
		}

		if (element->max_size.x > 0 && element->size.x > element->max_size.x) {
			element->size.x = element->max_size.x;
		}
	} else if (layout.anchors & UI_ANCHOR_LEFT) {
		element->position.x = inner_bounds.x;
	} else if (layout.anchors & UI_ANCHOR_RIGHT) {
		element->position.x = outer_bounds.x - element->size.x;
	}

	if (layout.anchors & UI_ANCHOR_CENTER) {
		if (!(layout.anchors & UI_ANCHOR_LEFT) && !(layout.anchors & UI_ANCHOR_RIGHT)) {
			element->position.x = ((outer_bounds.x + inner_bounds.x) / 2) - (element->size.x / 2);
		}

		if (!(layout.anchors & UI_ANCHOR_TOP) && !(layout.anchors & UI_ANCHOR_BOTTOM)) {
			element->position.y = ((outer_bounds.y + inner_bounds.y) / 2) - (element->size.y / 2);
		}
	}
}

static void ui_element_calculate_layout(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	switch (element->layout.mode) {
		case UI_LAYOUT_MODE_POSITION:
			element->position = element->layout.position;
			break;
		case UI_LAYOUT_MODE_ANCHOR:
			ui_element_calculate_anchors(element, outer_bounds, inner_bounds);
			break;
		case UI_LAYOUT_MODE_CONTAINER:
			// Container will determine the position of the element when drawn.
			break;
		default:
			ls_log_fatal("Unknown layout mode: %d\n", element->layout.mode);
			break;
	}
}

void ui_element_calculate_position(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	// Start by calculating the size of the element.
	// The element at this point will set its size to be the minimum size based ont the bounds.
	// It is acceptable to have a size of 0,0 at this point.
	switch (element->type) {
		case UI_ELEMENT_TYPE_LABEL: {
			ui_label_calculate_size(element, outer_bounds, inner_bounds);
		} break;
		case UI_ELEMENT_TYPE_VERTICAL_CONTAINER: {
			ui_vertical_container_calculate_size(element, outer_bounds, inner_bounds);
		} break;
		case UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER: {
			ui_horizontal_container_calculate_size(element, outer_bounds, inner_bounds);
		} break;
		default:
			ls_log_fatal("Unknown element type: %d\n", element->type);
			return;
	}

	ui_element_calculate_layout(element, outer_bounds, inner_bounds);
}