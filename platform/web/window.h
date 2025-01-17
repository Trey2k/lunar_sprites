#ifndef WEB_WINDOW_H
#define WEB_WINDOW_H

#include "platform/window.h"

#include "core/types/slice.h"

#include <emscripten/html5.h>

struct PlatformWindow {
	Vector2u size;

	Vector2u position;
	String title;

	bool fullscreen;
	bool hidden;
	bool root_window;

	char *canvas_id;

#if defined(OPENGL_ENABLED)
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context;
#endif // OPENGL_ENABLED

	const PlatformOS *os;
	const Renderer *renderer;

	Slice64 *events;

	InputManager *input_manager;
};

#endif // WEB_WINDOW_H