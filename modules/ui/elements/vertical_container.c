#include "vertical_container.h"

#include "core/debug.h"
#include "elements.h"
#include "modules/ui/elements.h"

static void ui_vertical_container_get_child_bounds(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);

	Slice32 *child_bounds = element->vertical_container.child_bounds;
	slice32_clear(child_bounds);

	size_t n_children = slice_get_size(element->vertical_container.children);
	uint32 child_y_space = ((outer_bounds.y - inner_bounds.y) / n_children) - (element->vertical_container.spacing * (n_children));
	uint32 y_offset = element->vertical_container.spacing;
	for (size_t i = 0; i < n_children; i++) {
		UIElement *child = slice_get(element->vertical_container.children, i).ptr;
		Vector2u child_inner_bounds = vec2u(inner_bounds.x, inner_bounds.y + y_offset);
		Vector2u child_outer_bounds = vec2u(outer_bounds.x, inner_bounds.y + y_offset + child_y_space);
		ui_element_calculate_position(child, child_outer_bounds, child_inner_bounds);
		y_offset += child_y_space + element->vertical_container.spacing;
		slice32_append(child_bounds, SLICE_VAL32(u32, child_inner_bounds.x));
		slice32_append(child_bounds, SLICE_VAL32(u32, child_inner_bounds.y));
		slice32_append(child_bounds, SLICE_VAL32(u32, child_outer_bounds.x));
		slice32_append(child_bounds, SLICE_VAL32(u32, child_outer_bounds.y));
	}
}

UIElement *ui_vertical_container_create(uint32 spacing, uint32 anchors) {
	UIElement *element = (UIElement *)ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_VERTICAL_CONTAINER;
	element->vertical_container.spacing = spacing;
	element->vertical_container.children = slice_create(16, false);
	element->vertical_container.child_bounds = slice32_create(16 * 4);

	element->size = vec2u(0, 0);

	element->min_size = vec2u(0, 0);
	element->max_size = vec2u(0, 0);

	element->anchors = anchors;

	return element;
}

void ui_vertical_container_destroy(UIVerticalContainer *container) {
	for (size_t i = 0; i < slice_get_size(container->children); i++) {
		UIElement *child = slice_get(container->children, i).ptr;
		ui_element_destroy(child);
	}

	slice_destroy(container->children);
	slice32_destroy(container->child_bounds);
}

void ui_vertical_container_add_child(UIElement *element, UIElement *child) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);
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
	ui_vertical_container_get_child_bounds(element, outer_bounds, inner_bounds);

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

	element->size = vec2u(max_width, total_height);

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

void ui_vertical_container_draw(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_VERTICAL_CONTAINER);
	UIVerticalContainer *vertical_container = &element->vertical_container;

	LS_ASSERT(vertical_container->child_bounds);

	for (size_t i = 0; i < slice_get_size(vertical_container->children); i++) {
		UIElement *child = slice_get(vertical_container->children, i).ptr;
		uint32 inner_x = slice32_get(vertical_container->child_bounds, i * 4).u32;
		uint32 inner_y = slice32_get(vertical_container->child_bounds, i * 4 + 1).u32;
		uint32 outer_x = slice32_get(vertical_container->child_bounds, i * 4 + 2).u32;
		uint32 outer_y = slice32_get(vertical_container->child_bounds, i * 4 + 3).u32;
		ui_draw_element(child, vec2u(outer_x, outer_y), vec2u(inner_x, inner_y));
	}
}