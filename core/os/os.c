#include "core/os/os.h"
#include "core/debug.h"
#include "core/memory.h"
#include "platform/os.h"

void os_init() {
	platform_os_init();
}

void os_deinit() {
	platform_os_deinit();
}

LSNativeDisplayType os_get_native_display() {
	return platform_get_native_display();
}

uint64 os_get_time() {
	return platform_get_time();
}

bool os_path_exists(String path) {
	return platform_path_exists(path);
}

bool os_path_is_directory(String path) {
	return platform_path_is_directory(path);
}

bool os_path_is_file(String path) {
	return platform_path_is_file(path);
}

char *os_path_add(String path1, String path2) {
	return platform_path_add(path1, path2);
}

char *os_path_get_directory(String path) {
	return platform_path_get_directory(path);
}

char *os_path_get_filename(String path) {
	return platform_path_get_filename(path);
}

char *os_path_get_extension(String path) {
	return platform_path_get_extension(path);
}

char *os_path_to_absolute(String path) {
	return platform_path_to_absolute(path);
}

Slice64 *os_list_directory(String path) {
	return platform_list_directory(path);
}

char *os_read_file(String path, size_t *size) {
	return platform_read_file(path, size);
}

bool os_write_file(String path, const void *data, size_t size) {
	return platform_write_file(path, data, size);
}

LSFile os_open_file(String path, String mode) {
	return platform_open_file(path, mode);
}

void os_close_file(LSFile file) {
	platform_close_file(file);
}

size_t os_read_file_data(LSFile file, void *data, size_t size) {
	return platform_read_file_data(file, data, size);
}

size_t os_write_file_data(LSFile file, const void *data, size_t size) {
	return platform_write_file_data(file, data, size);
}

size_t os_get_file_size(LSFile file) {
	return platform_get_file_size(file);
}

size_t os_get_file_position(LSFile file) {
	return platform_get_file_position(file);
}

void os_set_file_position(LSFile file, size_t position) {
	platform_set_file_position(file, position);
}

bool os_is_end_of_file(LSFile file) {
	return platform_is_end_of_file(file);
}

void os_set_working_directory(String path) {
	platform_set_working_directory(path);
}

char *os_get_working_directory() {
	return platform_path_to_absolute(".");
}

void *os_open_library(String path) {
	return platform_open_library(path);
}

void *os_get_library_symbol(void *library, String symbol) {
	return platform_get_library_symbol(library, symbol);
}