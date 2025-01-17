#include "extensions.h"

#include "core/debug.h"
#include "core/types/slice.h"

static void append_extension(Slice64 *extensions, String extension);

bool egl_has_extension(EGLDisplay egl_display, String extension) {
	Slice64 *extensions = egl_get_extensions(egl_display);
	bool has_extension = slice64_contains(extensions, SLICE_VAL64(str, extension), slice64_string_compare);
	slice64_destroy(extensions);
	return has_extension;
}

Slice64 *egl_get_extensions(EGLDisplay egl_display) {
	Slice64 *extensions = slice64_create(16, true);

	String ext = eglQueryString(egl_display, EGL_EXTENSIONS);
	LS_ASSERT(ext);
	append_extension(extensions, ext);

	return extensions;
}

static void append_extension(Slice64 *slice, String extensions) {
	int32 start = 0;
	for (int32 i = 0; i < ls_str_length(extensions); i++) {
		if (extensions[i] == ' ') {
			char *extension = ls_malloc(sizeof(char) * (i - start + 1));
			ls_memcpy(extension, extensions + start, i - start);
			extension[i - start] = '\0';
			slice64_append(slice, SLICE_VAL64(ptr, extension));
			start = i + 1;
		}
	}
}