#include "core/os/os.h"
#include "core/debug.h"
#include "core/memory.h"
#include "platform/os.h"

struct OS {
	PlatformOS *platform_os;
	PlatformOSInterface interface;
};

OS *ls_create_os(InputManager *input_manager) {
	OS *os = ls_malloc(sizeof(OS));
	os->platform_os = platform_create_os(input_manager);
	os->interface = platform_get_os_interface(os->platform_os);
	LS_ASSERT(os->interface.get_time);

	return os;
}

void ls_destroy_os(OS *os) {
	LS_ASSERT(os);

	platform_destroy_os(os->platform_os);
	ls_free(os);
}

LSNativeDisplayType os_get_native_display(const OS *os) {
	LS_ASSERT(os);

	return platform_get_native_display(os->platform_os);
}

const PlatformOS *os_get_platform_os(const OS *os) {
	LS_ASSERT(os);

	return os->platform_os;
}

uint64 os_get_time(const OS *os) {
	LS_ASSERT(os);

	return os->interface.get_time();
}

bool os_path_exists(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_exists(path);
}

bool os_path_is_directory(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_is_directory(path);
}

bool os_path_is_file(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_is_file(path);
}

char *os_path_add(const OS *os, String path1, String path2) {
	LS_ASSERT(os);

	return os->interface.path_add(path1, path2);
}

char *os_path_get_directory(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_get_directory(path);
}

char *os_path_get_filename(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_get_filename(path);
}

char *os_path_get_extension(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_get_extension(path);
}

char *os_path_to_absolute(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.path_to_absolute(path);
}

Slice *os_list_directory(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.list_directory(path);
}

char *os_read_file(const OS *os, String path, size_t *size) {
	LS_ASSERT(os);

	return os->interface.read_file(path, size);
}

bool os_write_file(const OS *os, String path, const void *data, size_t size) {
	LS_ASSERT(os);

	return os->interface.write_file(path, data, size);
}

void os_set_working_directory(const OS *os, String path) {
	LS_ASSERT(os);

	os->interface.set_working_directory(path);
}

char *os_get_working_directory(const OS *os) {
	LS_ASSERT(os);

	return os->interface.path_to_absolute(".");
}

void *os_open_library(const OS *os, String path) {
	LS_ASSERT(os);

	return os->interface.open_library(path);
}

void *os_get_library_symbol(const OS *os, void *library, String symbol) {
	LS_ASSERT(os);

	return os->interface.get_library_symbol(library, symbol);
}