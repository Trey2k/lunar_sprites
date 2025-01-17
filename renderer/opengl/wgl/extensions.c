#include "renderer/opengl/wgl/extensions.h"

#include "core/debug.h"
#include "core/memory.h"
#include "core/types/slice.h"

#include <glad/wgl.h>

static void append_extension(Slice64 *extensions, String extension);

void wgl_print_extensions(HDC device_context) {
	Slice64 *extensions = wgl_get_extensions(device_context);

	ls_printf("WGL Extensions:\n");

	for (size_t i = 0; i < slice64_get_size(extensions); i++) {
		String extension = slice64_get(extensions, i).str;
		ls_printf("\t%s\n", extension);
	}

	slice64_destroy(extensions);
}

bool wgl_has_extension(HDC device_context, String extension) {
	Slice64 *extensions = wgl_get_extensions(device_context);
	bool has_extension = slice64_contains(extensions, SLICE_VAL64(str, extension), slice64_string_compare);
	slice64_destroy(extensions);
	return has_extension;
}

Slice64 *wgl_get_extensions(HDC device_context) {
	Slice64 *extensions = slice64_create(16, true);

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