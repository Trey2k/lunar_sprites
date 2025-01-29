#ifndef MEMORY_H
#define MEMORY_H

#include "core/api.h"
#include "core/types/typedefs.h"

LS_EXPORT void *ls_malloc(size_t size);
LS_EXPORT void *ls_realloc(void *ptr, size_t size);
LS_EXPORT void *ls_calloc(size_t count, size_t size);

LS_EXPORT void ls_memcpy(void *dest, const void *src, size_t size);
LS_EXPORT void ls_memset(void *dest, int value, size_t size);
LS_EXPORT void ls_memmove(void *dest, const void *src, size_t size);

LS_EXPORT void ls_free(void *ptr);

#endif // MEMORY_H