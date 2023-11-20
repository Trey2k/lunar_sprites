#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

#include "core/input/input_manager.h"

#if defined(WEB_ENABLED)
typedef int32 LSNativeDisplayType;
#else
typedef void *LSNativeDisplayType;
#endif

typedef struct PlatformOS PlatformOS;

PlatformOS *platform_create_os(InputManager *input_manager);
void platform_destroy_os(PlatformOS *os);

LSNativeDisplayType platform_get_native_display(const PlatformOS *os);

#endif // PLATFORM_OS_H