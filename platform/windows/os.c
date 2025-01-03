#include "platform/windows/os.h"
#include "platform/windows/keymap.h"
#include "platform/windows/window.h"

#include "platform/os.h"

#include "core/core.h"

#include <stdio.h>

static int64 window_procedure(HWND native_window, uint32 message, uint64 w_param, int64 l_param);

PlatformOS *platform_create_os(InputManager *input_manager) {
	LS_ASSERT(input_manager);

	PlatformOS *os = ls_malloc(sizeof(PlatformOS));

	os->window_class.cbSize = sizeof(WNDCLASSEX);
	os->window_class.style = 0;
	os->window_class.lpfnWndProc = &window_procedure;
	os->window_class.cbClsExtra = 0;
	os->window_class.cbWndExtra = 0;
	os->window_class.hInstance = NULL;
	os->window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	os->window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	os->window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	os->window_class.lpszMenuName = NULL;
	os->window_class.lpszClassName = LS_WIN_CLASS_NAME;
	os->window_class.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	os->input_manager = input_manager;

	if (!RegisterClassEx(&os->window_class)) {
		ls_log_fatal("Failed to register window class!\n");
		return NULL;
	}

	return os;
}

void platform_destroy_os(PlatformOS *os) {
	ls_free(os);
}

LSNativeDisplayType platform_get_native_display(const PlatformOS *os) {
	return CreateCompatibleDC(NULL);
}

static int64 window_procedure(HWND native_window, uint32 message, uint64 w_param, int64 l_param) {
	PlatformWindow *window = (PlatformWindow *)GetWindowLongPtr(native_window, GWLP_USERDATA);
	if (!window) {
		ls_log(LOG_LEVEL_WARNING, "Window procedure called before window was created!\n");
		return DefWindowProc(native_window, message, w_param, l_param);
	}

	switch (message) {
		case WM_CREATE: {
			CREATESTRUCT *pCreate = (CREATESTRUCT *)l_param;
			window = (PlatformWindow *)pCreate->lpCreateParams;

			// Set the GWLP_USERDATA value
			SetWindowLongPtr(native_window, GWLP_USERDATA, (LONG_PTR)window);
		} break;

		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		} break;

		case WM_KEYDOWN: {
			input_handle_press(window->input_manager, win_map_key(w_param));
			return 0;
		} break;

		case WM_KEYUP: {
			input_handle_release(window->input_manager, win_map_key(w_param));
			return 0;
		} break;

		case WM_MOUSEMOVE: {
			input_handle_mouse_move(window->input_manager,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_LBUTTONDOWN: {
			input_handle_mouse_press(window->input_manager, LS_MOUSE_BUTTON_LEFT,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_RBUTTONDOWN: {
			input_handle_mouse_press(window->input_manager, LS_MOUSE_BUTTON_RIGHT,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_MBUTTONDOWN: {
			input_handle_mouse_press(window->input_manager, LS_MOUSE_BUTTON_MIDDLE,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_LBUTTONUP: {
			input_handle_mouse_release(window->input_manager, LS_MOUSE_BUTTON_LEFT,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_RBUTTONUP: {
			input_handle_mouse_release(window->input_manager, LS_MOUSE_BUTTON_RIGHT,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_MBUTTONUP: {
			input_handle_mouse_release(window->input_manager, LS_MOUSE_BUTTON_MIDDLE,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_MOUSELEAVE: {
			input_handle_mouse_leave(window->input_manager,
					vec2i(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
			return 0;
		} break;

		case WM_CLOSE: {
			input_handle_window_close(window->input_manager);
			return 0;
		} break;

		case WM_SIZE: {
			input_set_active_window(window->input_manager, window->parent);

			input_handle_resize(window->input_manager, vec2u(LOWORD(l_param), HIWORD(l_param)));

			input_set_active_window(window->input_manager, NULL);
			return 0;
		} break;
	};

	return DefWindowProc(native_window, message, w_param, l_param);
}

uint64 platform_get_time() {
	FILETIME file_time;
	GetSystemTimeAsFileTime(&file_time);
	ULARGE_INTEGER large_integer;
	large_integer.LowPart = file_time.dwLowDateTime;
	large_integer.HighPart = file_time.dwHighDateTime;
	return large_integer.QuadPart / 10;
}

bool platform_path_exists(String path) {
	DWORD attributes = GetFileAttributes(path);
	return attributes != INVALID_FILE_ATTRIBUTES;
}

bool platform_path_is_directory(String path) {
	DWORD attributes = GetFileAttributes(path);
	return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool platform_path_is_file(String path) {
	DWORD attributes = GetFileAttributes(path);
	return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

char *platform_path_add(String path1, String path2) {
	size_t path1_length = ls_str_length(path1);
	size_t path2_length = ls_str_length(path2);

	if (ls_str_ends_with(path1, "/") || ls_str_ends_with(path1, "\\")) {
		path1_length--;
	}

	if (ls_str_starts_with(path2, "/") || ls_str_starts_with(path2, "\\")) {
		path2++;
		path2_length--;
	}

	if (ls_str_ends_with(path2, "/") || ls_str_ends_with(path2, "\\")) {
		path2_length--;
	}

	const size_t path_size = path1_length + path2_length + 2;
	char *path = ls_malloc(path_size);
	ls_str_copy_to(path, path1, path1_length + 1);
	path[path1_length] = '/';
	ls_str_copy_to(path + path1_length + 1, path2, path2_length + 1);
	path[path_size - 1] = '\0';

	for (size_t i = 0; i < path_size; i++) {
		if (path[i] == '/') {
			path[i] = '\\';
		}
	}

	return path;
}

char *platform_path_get_directory(String path) {
	size_t length = ls_str_length(path);
	size_t i = length - 1;
	while (i > 0 && path[i] != '/' && path[i] != '\\') {
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
	while (i > 0 && path[i] != '/' && path[i] != '\\') {
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
	char *absolute_path = ls_malloc(MAX_PATH);
	if (!GetFullPathName(path, MAX_PATH, absolute_path, NULL)) {
		ls_free(absolute_path);
		return NULL;
	}

	return absolute_path;
}

Slice *platform_list_directory(String path) {
	Slice *files = slice_create(16, true);
	char *search_path = platform_path_add(path, "*");
	WIN32_FIND_DATA find_data;
	HANDLE find_handle = FindFirstFile(search_path, &find_data);
	ls_free(search_path);
	if (find_handle == INVALID_HANDLE_VALUE) {
		return files;
	}

	do {
		if (ls_str_equals(find_data.cFileName, ".") || ls_str_equals(find_data.cFileName, "..")) {
			continue;
		}

		slice_append(files, SLICE_VAL(str, ls_str_copy(find_data.cFileName)));
	} while (FindNextFile(find_handle, &find_data));

	FindClose(find_handle);

	return files;
}

char *platform_read_file(String path, size_t *size) {
	HANDLE file_handle = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	LARGE_INTEGER file_size;
	if (!GetFileSizeEx(file_handle, &file_size)) {
		CloseHandle(file_handle);
		return NULL;
	}

	char *data = ls_malloc(file_size.QuadPart);
	DWORD bytes_read;
	if (!ReadFile(file_handle, data, file_size.QuadPart, &bytes_read, NULL)) {
		CloseHandle(file_handle);
		ls_free(data);
		return NULL;
	}

	CloseHandle(file_handle);

	*size = bytes_read;
	return data;
}

bool platform_write_file(String path, const void *data, size_t size) {
	HANDLE file_handle = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD bytes_written;
	if (!WriteFile(file_handle, data, size, &bytes_written, NULL)) {
		CloseHandle(file_handle);
		return false;
	}

	CloseHandle(file_handle);

	return true;
}

LSFile platform_open_file(String path, String mode) {
	return fopen(path, mode);
}

void platform_close_file(LSFile file) {
	fclose(file);
}

size_t platform_read_file_data(LSFile file, void *data, size_t size) {
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
	if (!SetCurrentDirectory(path)) {
		ls_log(LOG_LEVEL_WARNING, "Failed to set working directory to %s\n", path);
	}
}

void *platform_open_library(String path) {
	return LoadLibrary(path);
}

void *platform_get_library_symbol(void *library, String symbol) {
	return GetProcAddress(library, symbol);
}