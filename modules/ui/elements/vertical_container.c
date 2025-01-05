#include "vertical_container.h"

#include "core/debug.h"
#include "elements.h"
#include "modules/ui/elements.h"

static void ui_vertical_container_child_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);

	size_t n_children = slice_get_size(element->vertical_container.children);
	uint32 y_offset = element->vertical_container.spacing;
	uint32 x_size = outer_bounds.x - inner_bounds.x;
	for (size_t i = 0; i < n_children; i++) {
		UIElement *child = slice_get(element->vertical_container.children, i).ptr;
		Vector2u child_inner_bounds = vec2u(inner_bounds.x, inner_bounds.y + y_offset);
		Vector2u child_outer_bounds = vec2u(outer_bounds.x, outer_bounds.y - element->vertical_container.spacing);
		child->max_size.x = x_size;
		ui_element_calculate_position(child, child_outer_bounds, child_inner_bounds);

		y_offset += child->size.y + element->vertical_container.spacing;
	}
}

UIElement *ui_vertical_container_create(uint32 spacing, UIAllignment alignment) {
	UIElement *element = (UIElement *)ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_VERTICAL_CONTAINER;
	element->vertical_container.spacing = spacing;
	element->vertical_container.children = slice_create(16, false);
	element->vertical_container.alignment = alignment;

	element->size = vec2u(0, 0);

	element->min_size = vec2u(0, 0);
	element->max_size = vec2u(0, 0);

	element->layout.mode = UI_LAYOUT_MODE_ANCHOR;
	element->layout.anchors = UI_ANCHOR_FILL;

	return element;
}

void ui_vertical_container_destroy(UIVerticalContainer *container) {
	for (size_t i = 0; i < slice_get_size(container->children); i++) {
		UIElement *child = slice_get(container->children, i).ptr;
		ui_element_destroy(child);
	}

	slice_destroy(container->children);
}

void ui_vertical_container_add_child(UIElement *element, UIElement *child) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);
	// TODO: Store the previous layout mode and restore it after the child is removed.
	child->layout.mode = UI_LAYOUT_MODE_CONTAINER;
	slice_append(element->vertical_container.children, SLICE_VAL(ptr, child));
}

void ui_vertical_container_remove_child(UIElement *element, UIElement *child) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);
	for (size_t i = 0; i < slice_get_size(element->vertical_container.children); i++) {
		if (slice_get(element->vertical_container.children, i).ptr == child) {
			slice_remove(element->vertical_container.children, i);
			break;
		}
	}
}

void ui_vertical_container_calculate_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);
	// Calculates the size of the children
	ui_vertical_container_child_size(element, outer_bounds, inner_bounds);

	uint32 max_width = 0;
	uint32 total_height = 0;
	for (size_t i = 0; i < slice_get_size(element->vertical_container.children); i++) {
		UIElement *child = slice_get(element->vertical_container.children, i).ptr;
		Vector2u size = ui_element_get_size(child);
		if (size.x > max_width) {
			max_width = size.x;
		}
		total_height += size.y + element->vertical_container.spacing;
	}

	// If max size is set, use it
	if (element->max_size.x > 0) {
		max_width = element->max_size.x;
	}

	if (element->max_size.y > 0) {
		total_height = element->max_size.y;
	}

	element->size = vec2u(max_width, total_height);

	if (element->size.x < element->min_size.x) {
		element->size.x = element->min_size.x;
	}

	if (element->size.y < element->min_size.y) {
		element->size.y = element->min_size.y;
	}
}

void ui_vertical_container_draw(UIElement *element) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);

	UIVerticalContainer *vertical_container = &element->vertical_container;
	switch (vertical_container->alignment) {
		case UI_ALIGNMENT_BEGIN: {
			Vector2u child_position = element->position;
			child_position.y += vertical_container->spacing;

			for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
				UIElement *child = slice_get(vertical_container->children, i).ptr;
				child->position = child_position;
				ui_draw_element(child);
				child_position.y += child->size.y + vertical_container->spacing;
			}
		} break;
		case UI_ALIGNMENT_CENTER: {
			uint32 total_height = 0;
			for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
				UIElement *child = slice_get(vertical_container->children, i).ptr;
				total_height += child->size.y + vertical_container->spacing;
			}

			uint32 y_offset = 0;
			if (element->size.y > total_height) {
				y_offset = (element->size.y - total_height) / 2;
			}

			Vector2u child_position = element->position;
			child_position.y += y_offset;

			for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
				UIElement *child = slice_get(vertical_container->children, i).ptr;
				child->position = child_position;
				ui_draw_element(child);
				child_position.y += child->size.y + vertical_container->spacing;
			}
		} break;
		case UI_ALIGNMENT_END: {
			uint32 total_height = 0;
			for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
				UIElement *child = slice_get(vertical_container->children, i).ptr;
				total_height += child->size.y + vertical_container->spacing;
			}

			uint32 y_offset = 0;
			if (element->size.y > total_height) {
				y_offset = element->size.y - total_height;
			}

			Vector2u child_position = element->position;
			child_position.y += y_offset;

			for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
				UIElement *child = slice_get(vertical_container->children, i).ptr;
				child->position = child_position;
				ui_draw_element(child);
				child_position.y += child->size.y + vertical_container->spacing;
			}
		} break;
		default:
			ls_log_fatal("Unknown alignment: %d\n", vertical_container->alignment);
			break;
	};
}

static void ui_vertical_container_handle_mouse_event(UIElement *element, Event *event) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);

	UIVerticalContainer *vertical_container = &element->vertical_container;
	for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
		UIElement *child = slice_get(vertical_container->children, i).ptr;
		if (event->mouse.position.x >= child->position.x && event->mouse.position.x <= child->position.x + child->size.x &&
				event->mouse.position.y >= child->position.y && event->mouse.position.y <= child->position.y + child->size.y) {
			ui_element_handle_event(child, event);
		}

		if (event->handled) {
			break;
		}
	}
}

void ui_vertical_container_handle_event(UIElement *element, Event *event) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);

	switch (event->type) {
		case EVENT_MOUSE: {
			ui_vertical_container_handle_mouse_event(element, event);
		} break;
		default:
			break;
	}
}