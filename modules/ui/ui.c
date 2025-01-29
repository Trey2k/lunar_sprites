#include "ui.h"

#include "core/debug.h"
#include "core/types/slice.h"
#include "elements/elements.h"

#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "renderer/window.h"

#include "main/lunar_sprites.h"

typedef struct {
	const LSWindow *window;

	Slice64 *elements;

	Slice32 *indices;
	BatchVertex *batch_vertices;
	size_t batch_vertices_size;
} UIRenderer;

static UIRenderer ui_renderer;

static void ui_on_update(float64 delta_time) {
	size_t n_elements = slice64_get_size(ui_renderer.elements);
	Vector2u inner_bounds = vec2u(0, 0);
	Vector2u outer_bounds = renderer_get_viewport_size();
	for (size_t i = 0; i < n_elements; i++) {
		UIElement *element = slice64_get(ui_renderer.elements, i).ptr;
		// Root elements bounds are the window size.

		// Updated element size and position
		ui_element_calculate_position(element, outer_bounds, inner_bounds);
		ui_draw_element(element);
	}
}

void ui_init(const LSWindow *window) {
	ui_renderer.window = window;
	ui_renderer.elements = slice64_create(16, false);
	ui_renderer.indices = slice32_create(128);
	ui_renderer.batch_vertices = ls_malloc(128 * sizeof(BatchVertex));
	ui_renderer.batch_vertices_size = 128;

	// Called before the main frame for now, might change later.
	ls_register_update_callback(ui_on_update);
}

void ui_deinit() {
	for (size_t i = 0; i < slice64_get_size(ui_renderer.elements); i++) {
		UIElement *element = slice64_get(ui_renderer.elements, i).ptr;
		ui_element_destroy(element);
	}

	slice64_destroy(ui_renderer.elements);
}

void ui_add_element(UIElement *element) {
	slice64_append(ui_renderer.elements, SLICE_VAL64(ptr, (void *)element));
}

void ui_remove_element(const UIElement *element) {
	size_t n_elements = slice64_get_size(ui_renderer.elements);
	for (size_t i = 0; i < n_elements; i++) {
		UIElement *elm = slice64_get(ui_renderer.elements, i).ptr;
		if (elm == element) {
			slice64_remove(ui_renderer.elements, i);
			ui_element_destroy(elm);
			break;
		}
	}
}