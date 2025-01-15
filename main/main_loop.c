#include "main_loop.h"

#include "main/lunar_sprites.h"
#include "renderer/window.h"

#include "renderer/batch_renderer.h"
#include "renderer/renderer.h"
#include "renderer/window.h"

typedef struct {
	Renderer *renderer;
	LSCore *core;

	LSMutex *render_mutex;
	LSThread *render_thread;
	float64 delta_time;
	uint64 last_frame_time;

	const LSWindow *root_window;
} MainLoop;

static MainLoop main_loop;

static void ls_render_loop(void *data);

void ls_main_loop_init(LSCore *core, Renderer *renderer, const LSWindow *root_window) {
	main_loop.root_window = root_window;
	main_loop.renderer = renderer;
	main_loop.core = core;

	// Start render thread, at this point all rendering should be done in the render loop.
	main_loop.render_mutex = os_mutex_create();
	// Make sure the render context is detached from the main thread.
	window_detach(main_loop.root_window);
	main_loop.render_thread = os_thread_create(ls_render_loop, NULL);
}

void ls_main_loop_deinit() {
	// Wait for render thread to finish first.
	os_thread_join(main_loop.render_thread);
	// Make the current context the main thread.
	window_make_current(main_loop.root_window);

	os_mutex_destroy(main_loop.render_mutex);
	os_thread_destroy(main_loop.render_thread);
}

static void ls_render_loop(void *data) {
	ls_log(LOG_LEVEL_INFO, "Starting render loop.\n");
	window_make_current(main_loop.root_window);

	while (!ls_should_stop()) {
		os_mutex_lock(main_loop.render_mutex);

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

		os_mutex_unlock(main_loop.render_mutex);
	}

	// Make sure to hand the render context back to the main thread.
	window_detach(main_loop.root_window);
}

void ls_main_loop() {
	core_poll(main_loop.core);

	if (main_loop.root_window) {
		window_poll(main_loop.root_window);
	}
}