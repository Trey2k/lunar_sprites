#include "renderer/opengl/wgl/extensions.h"

#include "core/debug.h"
#include "core/memory.h"
#include "core/types/slice.h"

#include <glad/wgl.h>

static void append_extension(Slice *extensions, String extension);
static bool extension_compare(const SliceValue a, const SliceValue b);

void wgl_print_extensions(HDC device_context) {
	Slice *extensions = wgl_get_extensions(device_context);

	ls_printf("WGL Extensions:\n");

	for (size_t i = 0; i < slice_get_size(extensions); i++) {
		String extension = slice_get(extensions, i).str;
		ls_printf("\t%s\n", extension);
	}

	slice_destroy(extensions);
}

bool wgl_has_extension(HDC device_context, String extension) {
	Slice *extensions = wgl_get_extensions(device_context);
	bool has_extension = slice_contains(extensions, (SliceValue){ .str = extension }, extension_compare);
	slice_destroy(extensions);
	return has_extension;
}

Slice *wgl_get_extensions(HDC device_context) {
	Slice *extensions = slice_create(16, true);

	if (wglGetExtensionsStringARB) {
		String arb = wglGetExtensionsStringARB(device_context);
		LS_ASSERT(arb);
		append_extension(extensions, arb);
		return extensions;
	}

	String ext = wglGetExtensionsStringEXT();
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

static bool extension_compare(const SliceValue a, const SliceValue b) {
	return ls_str_equals(a.str, b.str);
}