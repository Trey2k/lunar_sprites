#ifndef WINDOWS_OS_H
#define WINDOWS_OS_H

#include "core/os/os.h"
#include "platform/os.h"

#include <windows.h>
#include <windowsx.h>

static const char LS_WIN_CLASS_NAME[] = "LSWindowClass";

struct PlatformOS {
	WNDCLASSEX window_class;
};

#endif // WINDOWS_OS_H