#ifndef WEB_WINDOW_H
#define WEB_WINDOW_H

#include "platform/window.h"

#include <emscripten/html5.h>

struct PlatformWindow {
	int32 width;
	int32 height;
	String title;

	bool fullscreen;
	bool hidden;
};

#endif // WEB_WINDOW_H