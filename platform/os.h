#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

typedef struct PlatformOS PlatformOS;

PlatformOS *platform_create_os();
void platform_destroy_os(PlatformOS *os);
void *platform_get_native_display(const PlatformOS *os);

#endif // PLATFORM_OS_H