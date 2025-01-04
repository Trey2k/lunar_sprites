#include "horizontal_container.h"
#include "elements.h"

static void ui_horizontal_container_get_child_bounds(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);

	Slice32 *child_bounds = element->horizontal_container.child_bounds;
	slice32_clear(child_bounds);

	size_t n_children = slice_get_size(element->horizontal_container.children);
	uint32 child_x_space = ((outer_bounds.x - inner_bounds.x) / n_children) - (element->horizontal_container.spacing * (n_children));
	uint32 x_offset = element->horizontal_container.spacing;
	for (size_t i = 0; i < n_children; i++) {
		UIElement *child = slice_get(element->horizontal_container.children, i).ptr;
		Vector2u child_inner_bounds = vec2u(inner_bounds.x + x_offset, inner_bounds.y);
		Vector2u child_outer_bounds = vec2u(inner_bounds.x + x_offset + child_x_space, outer_bounds.y);
		ui_element_calculate_position(child, child_outer_bounds, child_inner_bounds);
		x_offset += child_x_space + element->horizontal_container.spacing;
		slice32_append(child_bounds, SLICE_VAL32(u32, child_inner_bounds.x));
		slice32_append(child_bounds, SLICE_VAL32(u32, child_inner_bounds.y));
		slice32_append(child_bounds, SLICE_VAL32(u32, child_outer_bounds.x));
		slice32_append(child_bounds, SLICE_VAL32(u32, child_outer_bounds.y));
	}
}

UIElement *ui_horizontal_container_create(uint32 spacing, uint32 anchors) {
	UIElement *element = (UIElement *)ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER;
	element->horizontal_container.spacing = spacing;
	element->horizontal_container.children = slice_create(16, false);
	element->horizontal_container.child_bounds = slice32_create(16 * 4);

	element->size = vec2u(0, 0);

	element->min_size = vec2u(0, 0);
	element->max_size = vec2u(0, 0);

	element->anchors = anchors;

	return element;
}

void ui_horizontal_container_destroy(UIHorizontalContainer *container) {
	for (size_t i = 0; i < slice_get_size(container->children); i++) {
		UIElement *child = slice_get(container->children, i).ptr;
		ui_element_destroy(child);
	}

	slice_destroy(container->children);
	slice32_destroy(container->child_bounds);
}

void ui_horizontal_container_add_child(UIElement *element, UIElement *child) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);
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
	ui_horizontal_container_get_child_bounds(element, outer_bounds, inner_bounds);

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

	element->size = vec2u(total_width, max_height);

	if (element->max_size.x > 0 && element->size.x > element->max_size.x) {
		element->size.x = element->max_size.x;
	}

	if (element->max_size.y > 0 && element->size.y > element->max_size.y) {
		element->size.y = element->max_size.y;
	}

	if (element->size.x < element->min_size.x) {
		element->size.x = element->min_size.x;
	}

	if (element->size.y < element->min_size.y) {
		element->size.y = element->min_size.y;
	}
}

void ui_horizontal_container_draw(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_HORIZONTAL_CONTAINER);
	UIHorizontalContainer *container = &element->horizontal_container;

	for (size_t i = 0; i < slice_get_size(container->children); i++) {
		UIElement *child = slice_get(container->children, i).ptr;
		uint32 inner_x = slice32_get(container->child_bounds, i * 4).u32;
		uint32 inner_y = slice32_get(container->child_bounds, i * 4 + 1).u32;
		uint32 outer_x = slice32_get(container->child_bounds, i * 4 + 2).u32;
		uint32 outer_y = slice32_get(container->child_bounds, i * 4 + 3).u32;

		ui_draw_element(child, vec2u(outer_x, outer_y), vec2u(inner_x, inner_y));
	}
}