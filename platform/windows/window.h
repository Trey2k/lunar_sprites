#ifndef WINDOWS_WINDOW_H
#define WINDOWS_WINDOW_H

#include "platform/window.h"

#include <windows.h>

struct PlatformWindow {
	HWND window;

	PlatformInput *input;
	size_t input_count;
};

#endif // WINDOWS_WINDOW_H