#ifndef OS_H
#define OS_H

#include "core/api.h"
#include "core/types/slice.h"
#include "core/types/typedefs.h"

#include "platform/os.h"

void os_init();
void os_deinit();

LSNativeDisplayType os_get_native_display();

// Returns current time in microseconds
LS_EXPORT uint64 os_get_time();

// Returns true if path is a file or directory
LS_EXPORT bool os_path_exists(String path);
// Returns true if path is a directory
LS_EXPORT bool os_path_is_directory(String path);
// Returns true if path is a file
LS_EXPORT bool os_path_is_file(String path);

// Returns a new string with path1 and path2 concatenated.
// Should handle path separators correctly. The String needs to be freed.
LS_EXPORT char *os_path_add(String path1, String path2);
// Returns the directory part of path. The String needs to be freed.
LS_EXPORT char *os_path_get_directory(String path);
// Returns the filename part of path. The String needs to be freed.
LS_EXPORT char *os_path_get_filename(String path);
// Returns the extension part of path. The String needs to be freed.
LS_EXPORT char *os_path_get_extension(String path);
// Returns an absolute path. The String needs to be freed.
LS_EXPORT char *os_path_to_absolute(String path);

// Returns a list of files and directories in path.
// Only the slice needs to be freed.
LS_EXPORT Slice64 *os_list_directory(String path);
// Reads a file into a buffer. The buffer needs to be freed.
LS_EXPORT char *os_read_file(String path, size_t *size);
// Writes data to a file. Returns true on success.
LS_EXPORT bool os_write_file(String path, const void *data, size_t size);

// Opens a file. Returns a handle to the file.
LS_EXPORT LSFile os_open_file(String path, String mode);
// Closes a file.
LS_EXPORT void os_close_file(LSFile file);
// Reads data from a file. Returns the number of bytes read.
LS_EXPORT size_t os_read_file_data(LSFile file, void *data, size_t size);
// Writes data to a file. Returns the number of bytes written.
LS_EXPORT size_t os_write_file_data(LSFile file, const void *data, size_t size);
// Returns the size of a file.
LS_EXPORT size_t os_get_file_size(LSFile file);
// Returns the current position in a file.
LS_EXPORT size_t os_get_file_position(LSFile file);
// Sets the current position in a file.
LS_EXPORT void os_set_file_position(LSFile file, size_t position);
// Returns true if the end of a file has been reached.
LS_EXPORT bool os_is_end_of_file(LSFile file);

// Sets the working directory.
LS_EXPORT void os_set_working_directory(String path);
LS_EXPORT char *os_get_working_directory();

// Opens a library. Returns a handle to the library.
LS_EXPORT void *os_open_library(String path);

// Imports a symbol from a library. Returns a pointer to the symbol.
LS_EXPORT void *os_get_library_symbol(void *library, String symbol);

#endif // OS_H