#include "core/lunar_sprites.h"
#include "core/input/input_manager.h"
#include "core/log/log.h"
#include "modules/register_module_types.h"

struct LunarSprites {
	EventManager *event_manager;
	InputManager *input_manager;
	OS *os;
	Renderer *renderer;

	LSWindow *root_window;
};

static struct LunarSprites ls;

void ls_init(RendererBackend renderer_backend) {
	ls.event_manager = ls_create_event_manager();
	LS_ASSERT(ls.event_manager);

	ls.os = ls_create_os();
	LS_ASSERT(ls.os);

	ls.input_manager = ls_create_input_manager(ls.event_manager);
	LS_ASSERT(ls.input_manager);

	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	ls.renderer = renderer_create(renderer_backend, ls.os);
	LS_ASSERT(ls.renderer);

	ls.root_window = ls_create_window("Lunar Sprites", 1280, 720);
	LS_ASSERT(ls.root_window);
}

void ls_poll() {
	input_poll(ls.input_manager);
	window_poll(ls.root_window);
}

void ls_deinit() {
	window_destroy(ls.root_window);

	renderer_destroy(ls.renderer);

	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);

	ls_destroy_os(ls.os);
	ls.os = NULL;

	input_manager_destroy(ls.input_manager);
	ls.input_manager = NULL;

	ls_destroy_event_manager(ls.event_manager);
	ls.event_manager = NULL;
}

const EventManager *ls_get_event_manager() {
	return ls.event_manager;
}

const InputManager *ls_get_input_manager() {
	return ls.input_manager;
}

const OS *ls_get_os() {
	return ls.os;
}

const Renderer *ls_get_renderer() {
	return ls.renderer;
}

const LSWindow *ls_get_root_window() {
	return ls.root_window;
}

void ls_handle_press(const LSWindow *window, LSKeycode keycode) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_press(ls.input_manager, window, keycode);
}

void ls_handle_release(const LSWindow *window, LSKeycode keycode) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_release(ls.input_manager, window, keycode);
}

void ls_handle_mouse_press(const LSWindow *window, LSMouseButton button, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_press(ls.input_manager, window, button, position);
}

void ls_handle_mouse_release(const LSWindow *window, LSMouseButton button, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_release(ls.input_manager, window, button, position);
}

void ls_handle_mouse_move(const LSWindow *window, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_move(ls.input_manager, window, position);
}

void ls_handle_mouse_enter(const LSWindow *window, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_enter(ls.input_manager, window, position);
}

void ls_handle_mouse_leave(const LSWindow *window, Vector2i position) {
	LS_ASSERT(window);
	LS_ASSERT(ls.input_manager);

	input_handle_mouse_leave(ls.input_manager, window, position);
}