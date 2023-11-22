#ifndef WEB_OS_H
#define WEB_OS_H

#include "core/os/os.h"
#include "platform/os.h"

#include "platform/web/window.h"

struct PlatformOS {
	InputManager *input_manager;
};

#endif // WEB_OS_H