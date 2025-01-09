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
	const Renderer *renderer;
	const LSWindow *window;

	Slice *elements;

	Slice32 *indices;
	BatchVertex *batch_vertices;
	size_t batch_vertices_size;

	InputManager *input_manager;
} UIRenderer;

static UIRenderer ui_renderer;

static void ui_on_update(float64 delta_time) {
	size_t n_elements = slice_get_size(ui_renderer.elements);
	Vector2u inner_bounds = vec2u(0, 0);
	Vector2u outer_bounds = window_get_size(ui_renderer.window);
	for (size_t i = 0; i < n_elements; i++) {
		UIElement *element = slice_get(ui_renderer.elements, i).ptr;
		// Root elements bounds are the window size.

		// Updated element size and position
		ui_element_calculate_position(element, outer_bounds, inner_bounds);
		ui_draw_element(element);
	}
}

static void ui_mouse_event_handler(Event *event) {
	Vector2u mouse_pos = event->mouse.position;
	for (size_t i = 0; i < slice_get_size(ui_renderer.elements); i++) {
		UIElement *element = slice_get(ui_renderer.elements, i).ptr;
		if (mouse_pos.x >= element->position.x && mouse_pos.x <= element->position.x + element->size.x &&
				mouse_pos.y >= element->position.y && mouse_pos.y <= element->position.y + element->size.y) {
			ui_element_handle_event(element, event);
		}

		if (event->handled) {
			break;
		}
	}
}

static void ui_event_handler(Event *event, void *user_data) {
	switch (event->type) {
		case EVENT_MOUSE: {
			ui_mouse_event_handler(event);
		} break;
		case EVENT_KEY: {
			// TODO: Handle key events
		} break;
		default:
			break;
	}
}

void ui_init(Renderer *renderer, LSCore *core, const LSWindow *window) {
	ui_renderer.input_manager = core_get_input_manager(core);
	ui_renderer.renderer = renderer;
	ui_renderer.window = window;
	ui_renderer.elements = slice_create(16, false);
	ui_renderer.indices = slice32_create(128);
	ui_renderer.batch_vertices = ls_malloc(128 * sizeof(BatchVertex));
	ui_renderer.batch_vertices_size = 128;

	// Called before the main frame for now, might change later.
	ls_register_update_callback(ui_on_update, true);
	event_manager_add_handler(core_get_event_manager(core), ui_event_handler, NULL);
}

void ui_deinit() {
	for (size_t i = 0; i < slice_get_size(ui_renderer.elements); i++) {
		UIElement *element = slice_get(ui_renderer.elements, i).ptr;
		ui_element_destroy(element);
	}

	slice_destroy(ui_renderer.elements);
}

void ui_add_element(UIElement *element) {
	slice_append(ui_renderer.elements, SLICE_VAL(ptr, (void *)element));
}

void ui_remove_element(const UIElement *element) {
	size_t n_elements = slice_get_size(ui_renderer.elements);
	for (size_t i = 0; i < n_elements; i++) {
		UIElement *elm = slice_get(ui_renderer.elements, i).ptr;
		if (elm == element) {
			slice_remove(ui_renderer.elements, i);
			ui_element_destroy(elm);
			break;
		}
	}
}

InputManager *ui_get_input_manager() {
	return ui_renderer.input_manager;
}