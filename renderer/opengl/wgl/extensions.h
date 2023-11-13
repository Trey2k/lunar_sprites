#ifndef WGL_EXTENSIONS_H
#define WGL_EXTENSIONS_H

#include "core/types/slice.h"
#include "core/types/string.h"

#include <windows.h>

void wgl_print_extensions(HDC device_context);

bool wgl_has_extension(HDC device_context, String extension);

// Returns a slice of char * strings
// Ownership of the slice is passed to the caller
Slice *wgl_get_extensions(HDC device_context);

#endif // WGL_EXTENSIONS_H