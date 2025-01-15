#include "main_loop.h"

#include "main/lunar_sprites.h"
#include "renderer/window.h"

#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"
#include "renderer/window.h"

typedef struct {
	Renderer *renderer;
	LSCore *core;

	float64 delta_time;
	uint64 last_frame_time;

	const LSWindow *root_window;
} MainLoop;

static MainLoop main_loop;

void ls_main_loop_init(LSCore *core, Renderer *renderer, const LSWindow *root_window) {
	main_loop.root_window = root_window;
	main_loop.renderer = renderer;
	main_loop.core = core;
}

void ls_main_loop_deinit() {
	// Nothing to do here.
}

void ls_main_loop() {
	core_poll(main_loop.core);

	window_update(main_loop.root_window);

	uint64 current_time = os_get_time();
	main_loop.delta_time = (current_time - main_loop.last_frame_time) / 1000000.0;
	main_loop.last_frame_time = current_time;

	renderer_clear(main_loop.renderer);
	ls_update(main_loop.delta_time);

	batch_renderer_end_frame();

	if (main_loop.root_window) {
		window_make_current(main_loop.root_window);
		window_swap_buffers(main_loop.root_window);
	}

	if (main_loop.root_window) {
		window_poll(main_loop.root_window);
	}
}