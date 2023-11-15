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

	Slice *events;
};

#endif // WEB_WINDOW_H