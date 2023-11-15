#ifndef EGL_EXTENSIONS_H
#define EGL_EXTENSIONS_H

#include "core/types/slice.h"
#include "core/types/string.h"

#include "renderer/opengl/egl/egl.h"

void egl_print_extensions(EGLDisplay egl_display);

bool egl_has_extension(EGLDisplay egl_display, String extension);

// Returns a slice of char * strings
// Ownership of the slice is passed to the caller
Slice *egl_get_extensions(EGLDisplay egl_display);

#endif // EGL_EXTENSIONS_H