#ifndef PATH_H
#define PATH_H

#include "core/types/string.h"

// Returns the files extension, or NULL if the path has no extension.
// The returned string is a substring of the path, so it should not be freed.
LS_EXPORT String ls_path_get_extension(String path);

#endif // PATH_H