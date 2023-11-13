#include "extensions.h"

#include "core/debug.h"
#include "core/types/slice.h"

static void append_extension(Slice *extensions, String extension);

bool egl_has_extension(EGLDisplay egl_display, String extension) {
	Slice *extensions = egl_get_extensions(egl_display);
	bool has_extension = slice_contains(extensions, SLICE_VAL(str, extension), slice_string_compare);
	slice_destroy(extensions);
	return has_extension;
}

Slice *egl_get_extensions(EGLDisplay egl_display) {
	Slice *extensions = slice_create(16, true);

	String ext = eglQueryString(egl_display, EGL_EXTENSIONS);
	LS_ASSERT(ext);
	append_extension(extensions, ext);

	return extensions;
}

static void append_extension(Slice *slice, String extensions) {
	int32 start = 0;
	for (int32 i = 0; i < ls_str_length(extensions); i++) {
		if (extensions[i] == ' ') {
			char *extension = ls_malloc(sizeof(char) * (i - start + 1));
			ls_memcpy(extension, extensions + start, i - start);
			extension[i - start] = '\0';
			slice_append(slice, (SliceValue){ .ptr = extension });
			start = i + 1;
		}
	}
}