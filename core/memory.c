#include "core/memory.h"

#include <stdarg.h>
#include <stdlib.h>

void *ls_malloc(size_t size) {
	return malloc(size);
}

void *ls_realloc(void *ptr, size_t size) {
	return realloc(ptr, size);
}

void *ls_calloc(size_t count, size_t size) {
	return calloc(count, size);
}

void ls_free(void *ptr) {
	free(ptr);
}