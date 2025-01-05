#include "horizontal_container.h"
#include "elements.h"

static void ui_horizontal_container_child_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);

	size_t n_children = slice_get_size(element->horizontal_container.children);
	uint32 x_offset = element->horizontal_container.spacing;
	uint32 y_size = outer_bounds.y - inner_bounds.y;
	for (size_t i = 0; i < n_children; i++) {
		UIElement *child = slice_get(element->horizontal_container.children, i).ptr;
		Vector2u child_inner_bounds = vec2u(inner_bounds.x + x_offset, inner_bounds.y);
		Vector2u child_outer_bounds = vec2u(outer_bounds.x - element->horizontal_container.spacing, outer_bounds.y);
		child->max_size.y = y_size;
		ui_element_calculate_position(child, child_outer_bounds, child_inner_bounds);

		x_offset += child->size.x + element->horizontal_container.spacing;
	}
}

UIElement *ui_horizontal_container_create(uint32 spacing, UIAllignment alignment) {
	UIElement *element = (UIElement *)ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER;
	element->horizontal_container.spacing = spacing;
	element->horizontal_container.children = slice_create(16, false);
	element->horizontal_container.alignment = alignment;

	element->size = vec2u(0, 0);

	element->min_size = vec2u(0, 0);
	element->max_size = vec2u(0, 0);

	element->layout.mode = UI_LAYOUT_MODE_ANCHOR;
	element->layout.anchors = UI_ANCHOR_FILL;

	return element;
}

void ui_horizontal_container_destroy(UIHorizontalContainer *container) {
	for (size_t i = 0; i < slice_get_size(container->children); i++) {
		UIElement *child = slice_get(container->children, i).ptr;
		ui_element_destroy(child);
	}

	slice_destroy(container->children);
}

void ui_horizontal_container_add_child(UIElement *element, UIElement *child) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);
	// TODO: Store the previous layout mode and restore it after the child is removed.
	child->layout.mode = UI_LAYOUT_MODE_ANCHOR;

	slice_append(element->horizontal_container.children, SLICE_VAL(ptr, child));
}

void ui_horizontal_container_remove_child(UIElement *element, UIElement *child) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);
	for (size_t i = 0; i < slice_get_size(element->horizontal_container.children); i++) {
		if (slice_get(element->horizontal_container.children, i).ptr == child) {
			slice_remove(element->horizontal_container.children, i);
			break;
		}
	}
}

void ui_horizontal_container_calculate_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);
	// Calculates the size of the children
	ui_horizontal_container_child_size(element, outer_bounds, inner_bounds);

	uint32 max_height = 0;
	uint32 total_width = 0;
	for (size_t i = 0; i < slice_get_size(element->horizontal_container.children); i++) {
		UIElement *child = slice_get(element->horizontal_container.children, i).ptr;
		Vector2u size = ui_element_get_size(child);
		if (size.y > max_height) {
			max_height = size.y;
		}
		total_width += size.x + element->horizontal_container.spacing;
	}

	// If max size is set, use it
	if (element->max_size.y > 0) {
		max_height = element->max_size.y;
	}

	if (element->max_size.x > 0) {
		total_width = element->max_size.x;
	}

	element->size = vec2u(total_width, max_height);

	if (element->size.x < element->min_size.x) {
		element->size.x = element->min_size.x;
	}

	if (element->size.y < element->min_size.y) {
		element->size.y = element->min_size.y;
	}
}

void ui_horizontal_container_draw(UIElement *element) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);

	UIHorizontalContainer *container = &element->horizontal_container;
	switch (container->alignment) {
		case UI_ALIGNMENT_BEGIN: {
			Vector2u child_position = element->position;
			child_position.x += container->spacing;

			for (size_t i = 0; i < slice_get_size(container->children); i++) {
				UIElement *child = slice_get(container->children, i).ptr;
				child->position = child_position;
				ui_draw_element(child);
				child_position.x += child->size.x + container->spacing;
			}
		} break;
		case UI_ALIGNMENT_CENTER: {
			uint32 total_width = 0;
			for (size_t i = 0; i < slice_get_size(container->children); i++) {
				UIElement *child = slice_get(container->children, i).ptr;
				total_width += child->size.x + container->spacing;
			}

			uint32 x_offset = (element->size.x - total_width) / 2;
			Vector2u child_position = element->position;
			child_position.x += x_offset;

			for (size_t i = 0; i < slice_get_size(container->children); i++) {
				UIElement *child = slice_get(container->children, i).ptr;
				child->position = child_position;
				ui_draw_element(child);
				child_position.x += child->size.x + container->spacing;
			}
		} break;
		case UI_ALIGNMENT_END: {
			uint32 total_width = 0;
			for (size_t i = 0; i < slice_get_size(container->children); i++) {
				UIElement *child = slice_get(container->children, i).ptr;
				total_width += child->size.x + container->spacing;
			}

			uint32 x_offset = element->size.x - total_width;
			Vector2u child_position = element->position;
			child_position.x += x_offset;

			for (size_t i = 0; i < slice_get_size(container->children); i++) {
				UIElement *child = slice_get(container->children, i).ptr;
				child->position = child_position;
				ui_draw_element(child);
				child_position.x += child->size.x + container->spacing;
			}
		} break;
		default:
			ls_log_fatal("Unknown alignment: %d\n", container->alignment);
			break;
	};
}