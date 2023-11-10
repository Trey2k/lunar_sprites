#ifndef WINDOWS_OS_H
#define WINDOWS_OS_H

#include "core/os/os.h"
#include "platform/os.h"

struct PlatformOS {
#if defined(MSVC_ENABLED)
	int32 dummy;
#endif // MSVC_ENABLED
};

#endif // WINDOWS_OS_H