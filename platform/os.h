#ifndef PLATFORM_OS_H
#define PLATFORM_OS_H

#include "core/input/input_manager.h"
#include "core/types/slice.h"

#if defined(WEB_ENABLED)
typedef int32 LSNativeDisplayType;
#else
typedef void *LSNativeDisplayType;
#endif

typedef struct PlatformOS PlatformOS;

PlatformOS *platform_create_os(InputManager *input_manager);
void platform_destroy_os(PlatformOS *os);

LSNativeDisplayType platform_get_native_display(const PlatformOS *os);

uint64 platform_get_time();
bool platform_path_exists(String path);
bool platform_path_is_directory(String path);
bool platform_path_is_file(String path);

char *platform_path_add(String path1, String path2);
char *platform_path_get_directory(String path);
char *platform_path_get_filename(String path);
char *platform_path_get_extension(String path);
char *platform_path_to_absolute(String path);

Slice64 *platform_list_directory(String path);
char *platform_read_file(String path, size_t *size);
bool platform_write_file(String path, const void *data, size_t size);

LSFile platform_open_file(String path, String mode);
void platform_close_file(LSFile file);
size_t platform_read_file_data(LSFile file, void *data, size_t size);
size_t platform_write_file_data(LSFile file, const void *data, size_t size);
size_t platform_get_file_size(LSFile file);
size_t platform_get_file_position(LSFile file);
void platform_set_file_position(LSFile file, size_t position);
bool platform_is_end_of_file(LSFile file);

void platform_set_working_directory(String path);
char *platform_get_working_directory();

void *platform_open_library(String path);
void *platform_get_library_symbol(void *library, String symbol);

#endif // PLATFORM_OS_H