#include "core/memory.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void *ls_malloc(size_t size) {
	return malloc(size);
}

void *ls_realloc(void *ptr, size_t size) {
	return realloc(ptr, size);
}

void *ls_calloc(size_t count, size_t size) {
	return calloc(count, size);
}

void ls_memset(void *dest, int32 value, size_t size) {
	memset(dest, value, size);
}

void ls_memcpy(void *dest, const void *src, size_t size) {
	memcpy(dest, src, size);
}

void ls_memmove(void *dest, const void *src, size_t size) {
	memmove(dest, src, size);
}

void ls_free(void *ptr) {
	free(ptr);
}