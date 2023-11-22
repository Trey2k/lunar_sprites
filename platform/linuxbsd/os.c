#include "platform/linuxbsd/os.h"

#include "core/debug.h"
#include "core/log.h"
#include "core/memory.h"

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

static uint64 linuxbsd_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64)tv.tv_sec * 1000000 + (uint64)tv.tv_usec;
}

static bool linuxbsd_path_exists(String path) {
	struct stat st;
	return stat(path, &st) == 0;
}

static bool linuxbsd_path_is_directory(String path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}

	return S_ISDIR(st.st_mode);
}

static bool linuxbsd_path_is_file(String path) {
	struct stat st;
	if (stat(path, &st) != 0) {
		return false;
	}

	return S_ISREG(st.st_mode);
}

static char *linuxbsd_path_add(String path1, String path2) {
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

static char *linuxbsd_path_get_directory(String path) {
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

static char *linuxbsd_path_get_filename(String path) {
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

static char *linuxbsd_path_get_extension(String path) {
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

static char *linuxbsd_path_to_absolute(String path) {
	char *absolute_path = realpath(path, NULL);
	if (!absolute_path) {
		return NULL;
	}

	return absolute_path;
}

static Slice *linuxbsd_list_directory(String path) {
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

static char *linuxbsd_read_file(String path, size_t *size) {
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

static bool linuxbsd_write_file(String path, const void *data, size_t size) {
	FILE *file = fopen(path, "wb");
	if (!file) {
		return false;
	}

	fwrite(data, 1, size, file);
	fclose(file);

	return true;
}

static void linuxbsd_set_working_directory(String path) {
	if (chdir(path) != 0) {
		ls_log(LOG_LEVEL_INFO, "Failed to set working directory to %s\n", path);
	}
}

static char *linuxbsd_get_working_directory() {
	return getcwd(NULL, 0);
}

static void *linuxbsd_open_library(String path) {
	return dlopen(path, RTLD_LAZY);
}

static void *linuxbsd_get_library_symbol(void *library, String symbol) {
	return dlsym(library, symbol);
}

PlatformOSInterface platform_get_os_interface(const PlatformOS *os) {
	PlatformOSInterface interface;
	interface.get_time = linuxbsd_get_time;
	interface.path_exists = linuxbsd_path_exists;
	interface.path_is_directory = linuxbsd_path_is_directory;
	interface.path_is_file = linuxbsd_path_is_file;
	interface.path_add = linuxbsd_path_add;
	interface.path_get_directory = linuxbsd_path_get_directory;
	interface.path_get_filename = linuxbsd_path_get_filename;
	interface.path_get_extension = linuxbsd_path_get_extension;
	interface.list_directory = linuxbsd_list_directory;
	interface.read_file = linuxbsd_read_file;
	interface.write_file = linuxbsd_write_file;
	interface.set_working_directory = linuxbsd_set_working_directory;
	interface.path_to_absolute = linuxbsd_path_to_absolute;
	interface.get_working_directory = linuxbsd_get_working_directory;
	interface.open_library = linuxbsd_open_library;
	interface.get_library_symbol = linuxbsd_get_library_symbol;
	return interface;
}