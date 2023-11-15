#include "main/lunar_sprites.h"

#include "platform/web/webgl.h"

#include <emscripten/emscripten.h>

static void main_loop() {
	ls_main_loop();
	if (ls_should_quit()) {
		emscripten_cancel_main_loop();

		// Code after emscripten_set_main_loop is never executed even with simulate_mainloop = true.
		// So we must cleanup here.
		// See https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop
		ls_main_deinit();
	}
}

int32 main(int argc, char *argv[]) {
	ls_main_init(argc, argv);

	emscripten_set_main_loop(main_loop, 0, true);

	return 0;
}