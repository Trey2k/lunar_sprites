#include "main_loop.h"

#include "main/lunar_sprites.h"
#include "renderer/window.h"

#include "renderer/batch_renderer.h"
#include "renderer/window.h"

struct {
	float64 delta_time;
	uint64 last_frame_time;

	LSWindow *root_window;
} main_loop = { 0 };

void ls_main_loop_init(LSWindow *root_window) {
	main_loop.root_window = root_window;
}

void ls_draw_frame() {
	uint64 current_time = os_get_time();
	main_loop.delta_time = (current_time - main_loop.last_frame_time) / 1000000.0;
	main_loop.last_frame_time = current_time;

	renderer_clear();
	ls_update(main_loop.delta_time);

	batch_renderer_end_frame();

	if (main_loop.root_window) {
		window_make_current(main_loop.root_window);
		window_swap_buffers(main_loop.root_window);
	}
}

void ls_main_loop() {
	input_poll();

	if (main_loop.root_window) {
		window_poll(main_loop.root_window);
	}

	ls_draw_frame();
}