#include "platform/linuxbsd/os.h"

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

#if defined(WAYLAND_ENABLED)
	WaylandServer *wayland_server = NULL;
	wayland_server = wayland_server_create();
	if (wayland_server) {
		os->display_server = DISPLAY_SERVER_WAYLAND;
		os->wayland_server = wayland_server;
	}
#endif

#if defined(X11_ENABLED)
	X11Server *x11_server = NULL;
	x11_server = x11_server_create(input_manager);
	if (x11_server) {
		os->display_server = DISPLAY_SERVER_X11;
		os->x11_server = x11_server;
	}
#endif

	if (os->display_server == DISPLAY_SERVER_NONE) {
		ls_log_fatal("Failed to create display server\n");
		return NULL;
	}

	return os;
}

void platform_destroy_os(PlatformOS *os) {
	switch (os->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND:
			wayland_server_destroy(os->wayland_server);
			break;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11:
			x11_server_destroy(os->x11_server);
			break;
#endif
		default:
			ls_log_fatal("Unknown display type: %d\n", os->display_server);
	};
}

LSNativeDisplayType platform_get_native_display(const PlatformOS *os) {
	switch (os->display_server) {
#if defined(WAYLAND_ENABLED)
		case DISPLAY_SERVER_WAYLAND:
			return os->wayland_server->display;
#endif

#if defined(X11_ENABLED)
		case DISPLAY_SERVER_X11:
			return os->x11_server->display;
#endif

		default:
			ls_log_fatal("Unknown display type: %d\n", os->display_server);
			return NULL;
	};
}

DisplayServer os_linuxbsd_get_display_server(const PlatformOS *os) {
	return os->display_server;
}

#if defined(WAYLAND_ENABLED)
WaylandServer *os_linuxbsd_get_wayland_server(const PlatformOS *os) {
	return os->wayland_server;
}
#endif

#if defined(X11_ENABLED)
X11Server *os_linuxbsd_get_x11_server(const PlatformOS *os) {
	return os->x11_server;
}
#endif

uint64 platform_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64)tv.tv_sec * 1000000 + (uint64)tv.tv_usec;
}

bool platform_path_exists(String path) {
	struct stat st;
	return stat(path, &st) == 0;
}

bool platform_path_is_directory(String path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}

	return S_ISDIR(st.st_mode);
}

bool platform_path_is_file(String path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}

	return S_ISREG(st.st_mode);
}

char *platform_path_add(String path1, String path2) {
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

char *platform_path_get_directory(String path) {
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

char *platform_path_get_filename(String path) {
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

char *platform_path_get_extension(String path) {
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

char *platform_path_to_absolute(String path) {
	char *absolute_path = realpath(path, NULL);
	if (!absolute_path) {
		return NULL;
	}

	return absolute_path;
}

Slice64 *platform_list_directory(String path) {
	DIR *dir = opendir(path);
	if (!dir) {
		return NULL;
	}

	Slice64 *files = slice64_create(16, true);

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type != DT_REG ||
				ls_str_equals(entry->d_name, ".") ||
				ls_str_equals(entry->d_name, "..")) {
			continue;
		}

		char *file_name = ls_str_copy(entry->d_name);
		slice64_append(files, SLICE_VAL64(ptr, file_name));
	}
	closedir(dir);

	return files;
}

char *platform_read_file(String path, size_t *size) {
	FILE *file = fopen(path, "rb");
	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *data = ls_malloc(file_size + 1);
	fread(data, 1, file_size, file);
	data[file_size] = '\0';

	fclose(file);

	if (size) {
		*size = file_size;
	}

	return data;
}

bool platform_write_file(String path, const void *data, size_t size) {
	FILE *file = fopen(path, "wb");
	if (!file) {
		return false;
	}

	fwrite(data, 1, size, file);
	fclose(file);

	return true;
}

LSFile platform_open_file(String path, String mode) {
	return fopen(path, mode);
}

void platform_close_file(LSFile file) {
	fclose(file);
}

size_t platform_read_file_data(LSFile file, void *data, size_t size) {
	LS_ASSERT(size <= platform_get_file_size(file));
	return fread(data, 1, size, file);
}

size_t platform_write_file_data(LSFile file, const void *data, size_t size) {
	return fwrite(data, 1, size, file);
}

size_t platform_get_file_size(LSFile file) {
	size_t current_position = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, current_position, SEEK_SET);

	return size;
}

size_t platform_get_file_position(LSFile file) {
	return ftell(file);
}

void platform_set_file_position(LSFile file, size_t position) {
	fseek(file, position, SEEK_SET);
}

bool platform_is_end_of_file(LSFile file) {
	return feof(file);
}

void platform_set_working_directory(String path) {
	if (chdir(path) != 0) {
		ls_log(LOG_LEVEL_INFO, "Failed to set working directory to %s\n", path);
	}
}

char *platform_get_working_directory() {
	return getcwd(NULL, 0);
}

void *platform_open_library(String path) {
	return dlopen(path, RTLD_LAZY);
}

void *platform_get_library_symbol(void *library, String symbol) {
	return dlsym(library, symbol);
}