#include "core/path.h"

#include "core/debug.h"
#include "core/memory.h"

String ls_path_get_extension(String path) {
	LS_ASSERT(path);

	char *ext = ls_str_char(path, '.');
	if (ext == NULL) {
		return NULL;
	}

	return ext + 1;
}