#include "main/main.h"

#include "core/input/input_manager.h"
#include "core/input/keycodes.h"
#include "core/lunar_sprites.h"
#include "core/window/window.h"

void main_event_handler(Event *event, void *user_data);

void main_start() {
	ls_init(RENDERER_BACKEND_OPENGL3);

	const LSWindow *root_window = ls_get_root_window();
	const InputManager *input_manager = ls_get_input_manager();

	while (!input_is_key_just_released(input_manager, LS_KEY_ESCAPE)) {
		ls_poll();
	}

	ls_deinit();
}