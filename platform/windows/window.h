#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H

#include "platform/window.h"

#include <windows.h>

struct PlatformWindow {
	HWND window;

	Vector2u size;
	Vector2u min_size;
	Vector2u max_size;
	String title;

	bool fullscreen;
	bool hidden;

	InputManager *input_manager;
	LSWindow *parent;
};

#endif // WINDOWS_WINDOW_H