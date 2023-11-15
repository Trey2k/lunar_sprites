#include "js_methods.h"

#include <emscripten/emscripten.h>

void new_canvas(String canvas_id, int32 width, int32 height, int32 x, int32 y) {
	EM_ASM({ Module.new_canvas(UTF8ToString($0), $1, $2, $3, $4); }, canvas_id, width, height, x, y);
}

void delete_canvas(String canvas_id) {
	EM_ASM({ Module.delete_canvas(UTF8ToString($0)); }, canvas_id);
}

void set_canvas_size(String canvas_id, int32 width, int32 height) {
	EM_ASM({ Module.set_canvas_size(UTF8ToString($0), $1, $2); }, canvas_id, width, height);
}

void set_canvas_focused(String canvas_id, bool focused) {
	EM_ASM({ Module.set_canvas_focused(UTF8ToString($0), $1); }, canvas_id, focused);
}

void set_canvas_position(String canvas_id, int32 x, int32 y) {
	EM_ASM({ Module.set_canvas_position(UTF8ToString($0), $1, $2); }, canvas_id, x, y);
}

void set_canvas_visibility(String canvas_id, bool visible) {
	EM_ASM({ Module.set_canvas_visibility(UTF8ToString($0), $1); }, canvas_id, visible);
}

void set_canvas_fullscreen(String canvas_id, bool fullscreen) {
	EM_ASM({ Module.set_canvas_fullscreen(UTF8ToString($0), $1); }, canvas_id, fullscreen);
}