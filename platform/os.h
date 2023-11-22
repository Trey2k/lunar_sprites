#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

#include "core/input/input_manager.h"
#include "core/types/slice.h"

typedef struct {
	uint64 (*get_time)();
	bool (*path_exists)(String path);
	bool (*path_is_directory)(String path);
	bool (*path_is_file)(String path);

	char *(*path_add)(String path1, String path2);
	char *(*path_get_directory)(String path);
	char *(*path_get_filename)(String path);
	char *(*path_get_extension)(String path);
	char *(*path_to_absolute)(String path);

	Slice *(*list_directory)(String path);
	char *(*read_file)(String path, size_t *size);
	bool (*write_file)(String path, const void *data, size_t size);

	void (*set_working_directory)(String path);
	char *(*get_working_directory)();

	void *(*open_library)(String path);
	void *(*get_library_symbol)(void *library, String symbol);
} PlatformOSInterface;

#if defined(WEB_ENABLED)
typedef int32 LSNativeDisplayType;
#else
typedef void *LSNativeDisplayType;
#endif

typedef struct PlatformOS PlatformOS;

PlatformOS *platform_create_os(InputManager *input_manager);
void platform_destroy_os(PlatformOS *os);

LSNativeDisplayType platform_get_native_display(const PlatformOS *os);
PlatformOSInterface platform_get_os_interface(const PlatformOS *os);

#endif // PLATFORM_OS_H