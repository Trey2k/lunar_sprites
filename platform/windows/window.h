#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H

#include "platform/window.h"

#include <windows.h>

struct PlatformWindow {
	HWND window;

	int32 width;
	int32 height;
	String title;

	bool fullscreen;
	bool hidden;

	InputManager *input_manager;
	LSWindow *parent;
};

#endif // WINDOWS_WINDOW_H