#include "platform/web/webgl.h"

#include "platform/web/js_methods.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

void webgl_init_canvas(PlatformWindow *window) {
	if (!window->root_window) {
		new_canvas(window->canvas_id, window->size.x, window->size.y, window->position.x, window->position.y);
	} else {
		set_canvas_size(window->canvas_id, window->size.x, window->size.y);
		set_canvas_position(window->canvas_id, window->position.x, window->position.y);
	}

	EmscriptenWebGLContextAttributes attrs;
	emscripten_webgl_init_context_attributes(&attrs);
	attrs.majorVersion = 2; // for WebGL 2.0 / OpenGL ES 3.0
	attrs.minorVersion = 0;
	window->webgl_context = emscripten_webgl_create_context(window->canvas_id, &attrs);
	emscripten_webgl_make_context_current(window->webgl_context);
}

void webgl_deinit_canvas(PlatformWindow *window) {
	emscripten_webgl_destroy_context(window->webgl_context);
}
