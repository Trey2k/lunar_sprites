#ifndef WEB_WINDOW_H
#define WEB_WINDOW_H

#include "platform/window.h"

#include "core/types/slice.h"

#include <emscripten/html5.h>

struct PlatformWindow {
	int32 width;
	int32 height;

	Vector2i position;
	String title;

	bool fullscreen;
	bool hidden;
	bool root_window;

	char *canvas_id;

#if defined(OPENGL_ENABLED)
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context;
#endif // OPENGL_ENABLED

	const PlatformOS *os;

	Slice *keydown_codes;
	Slice *keyup_codes;

	Slice32 *mousedown_codes;
	Slice128 *mousedown_positions;
	Slice32 *mouseup_codes;
	Slice128 *mouseup_positions;

	Slice128 *mouse_move_positions;
	Slice128 *mouse_enter_positions;
	Slice128 *mouse_leave_positions;
};

#endif // WEB_WINDOW_H