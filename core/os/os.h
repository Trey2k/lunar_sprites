#ifndef OS_H
#define OS_H

#include "core/api.h"
#include "core/input/input_manager.h"
#include "core/types/slice.h"
#include "core/types/typedefs.h"

#include "platform/os.h"

typedef struct OS OS;

OS *ls_create_os(InputManager *input_manager);
void ls_destroy_os(OS *os);

LSNativeDisplayType os_get_native_display(const OS *os);
const PlatformOS *os_get_platform_os(const OS *os);

// Returns current time in microseconds
LS_EXPORT uint64 os_get_time(const OS *os);

// Returns true if path is a file or directory
LS_EXPORT bool os_path_exists(const OS *os, String path);
// Returns true if path is a directory
LS_EXPORT bool os_path_is_directory(const OS *os, String path);
// Returns true if path is a file
LS_EXPORT bool os_path_is_file(const OS *os, String path);

// Returns a new string with path1 and path2 concatenated.
// Should handle path separators correctly. The String needs to be freed.
LS_EXPORT char *os_path_add(const OS *os, String path1, String path2);
// Returns the directory part of path. The String needs to be freed.
LS_EXPORT char *os_path_get_directory(const OS *os, String path);
// Returns the filename part of path. The String needs to be freed.
LS_EXPORT char *os_path_get_filename(const OS *os, String path);
// Returns the extension part of path. The String needs to be freed.
LS_EXPORT char *os_path_get_extension(const OS *os, String path);
// Returns an absolute path. The String needs to be freed.
LS_EXPORT char *os_path_to_absolute(const OS *os, String path);

// Returns a list of files and directories in path.
// Only the slice needs to be freed.
LS_EXPORT Slice *os_list_directory(const OS *os, String path);
// Reads a file into a buffer. The buffer needs to be freed.
LS_EXPORT char *os_read_file(const OS *os, String path, size_t *size);
// Writes data to a file. Returns true on success.
LS_EXPORT bool os_write_file(const OS *os, String path, const void *data, size_t size);

// Sets the working directory.
LS_EXPORT void os_set_working_directory(const OS *os, String path);
LS_EXPORT char *os_get_working_directory(const OS *os);

// Opens a library. Returns a handle to the library.
LS_EXPORT void *os_open_library(const OS *os, String path);

// Imports a symbol from a library. Returns a pointer to the symbol.
LS_EXPORT void *os_get_library_symbol(const OS *os, void *library, String symbol);

#endif // OS_H