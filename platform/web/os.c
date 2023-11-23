#include "platform/web/os.h"
#include "platform/web/keymap.h"

#include "core/core.h"

#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

PlatformOS *platform_create_os(InputManager *input_manager) {
	PlatformOS *os = ls_malloc(sizeof(PlatformOS));
	os->input_manager = input_manager;
	keymap_init();

	return os;
}

void platform_destroy_os(PlatformOS *os) {
	keymap_deinit();
	ls_free(os);
}

LSNativeDisplayType platform_get_native_display(const PlatformOS *os) {
	return 0;
}

static uint64 web_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64)tv.tv_sec * 1000000 + (uint64)tv.tv_usec;
}

static bool web_path_exists(String path) {
	struct stat st;
	return stat(path, &st) == 0;
}

static bool web_path_is_directory(String path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISDIR(st.st_mode);
}

static bool web_path_is_file(String path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISREG(st.st_mode);
}

static char *web_path_add(String path1, String path2) {
	size_t path1_length = ls_str_length(path1);
	size_t path2_length = ls_str_length(path2);

	if (ls_str_ends_with(path1, "/")) {
		path1_length--;
	}

	if (ls_str_starts_with(path2, "/")) {
		path2++;
		path2_length--;
	}

	if (ls_str_ends_with(path2, "/")) {
		path2_length--;
	}

	char *path = ls_malloc(path1_length + path2_length + 2);
	ls_str_copy_to(path, path1, path1_length + 1);
	path[path1_length] = '/';
	ls_str_copy_to(path + path1_length + 1, path2, path2_length + 1);
	path[path1_length + path2_length + 1] = '\0';

	return path;
}

static char *web_path_get_directory(String path) {
	size_t length = ls_str_length(path);
	size_t i = length - 1;
	while (i > 0 && path[i] != '/') {
		i--;
	}

	if (i == 0) {
		return NULL;
	}

	char *directory = ls_malloc(i + 1);
	ls_str_copy_to(directory, path, i + 1);
	directory[i] = '\0';

	return directory;
}

static char *web_path_get_filename(String path) {
	size_t length = ls_str_length(path);
	size_t i = length - 1;
	while (i > 0 && path[i] != '/') {
		i--;
	}

	if (i == 0) {
		return NULL;
	}

	char *filename = ls_malloc(length - i);
	ls_str_copy_to(filename, path + i + 1, length - i);
	filename[length - i - 1] = '\0';

	return filename;
}

static char *web_path_get_extension(String path) {
	size_t length = ls_str_length(path);
	size_t i = length - 1;
	while (i > 0 && path[i] != '.') {
		i--;
	}

	if (i == 0) {
		return NULL;
	}

	char *extension = ls_malloc(length - i);
	ls_str_copy_to(extension, path + i + 1, length - i);
	extension[length - i - 1] = '\0';

	return extension;
}

static char *web_path_to_absolute(String path) {
	char *absolute_path = realpath(path, NULL);
	if (!absolute_path) {
		return NULL;
	}

	return absolute_path;
}

static Slice *web_list_directory(String path) {
	DIR *dir = opendir(path);
	if (!dir) {
		return NULL;
	}

	Slice *files = slice_create(16, true);

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (ls_str_equals(entry->d_name, ".") || ls_str_equals(entry->d_name, "..")) {
			continue;
		}

		char *file_name = ls_str_copy(entry->d_name);
		slice_append(files, SLICE_VAL(ptr, file_name));
	}
	closedir(dir);

	return files;
}

static char *web_read_file(String path, size_t *size) {
	FILE *file = fopen(path, "rb");
	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = ls_malloc(file_size);
	fread(buffer, 1, file_size, file);
	fclose(file);

	*size = file_size;
	return buffer;
}

static bool web_write_file(String path, const void *data, size_t size) {
	FILE *file = fopen(path, "wb");
	if (!file) {
		return false;
	}

	fwrite(data, 1, size, file);
	fclose(file);

	return true;
}

static void web_set_working_directory(String path) {
	if (chdir(path) != 0) {
		ls_log(LOG_LEVEL_INFO, "Failed to set working directory to %s\n", path);
	}
}

static char *web_get_working_directory() {
	return getcwd(NULL, 0);
}

static void *web_open_library(String path) {
	return dlopen(path, RTLD_LAZY);
}

static void *web_get_library_symbol(void *library, String name) {
	return dlsym(library, name);
}

PlatformOSInterface platform_get_os_interface(const PlatformOS *os) {
	PlatformOSInterface interface;
	interface.get_time = web_get_time;
	interface.path_exists = web_path_exists;
	interface.path_is_directory = web_path_is_directory;
	interface.path_is_file = web_path_is_file;
	interface.path_add = web_path_add;
	interface.path_get_directory = web_path_get_directory;
	interface.path_get_filename = web_path_get_filename;
	interface.path_get_extension = web_path_get_extension;
	interface.path_to_absolute = web_path_to_absolute;
	interface.list_directory = web_list_directory;
	interface.read_file = web_read_file;
	interface.write_file = web_write_file;
	interface.set_working_directory = web_set_working_directory;
	interface.get_working_directory = web_get_working_directory;
	interface.open_library = web_open_library;
	interface.get_library_symbol = web_get_library_symbol;
	return interface;
}