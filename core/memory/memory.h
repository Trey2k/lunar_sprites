#ifndef MEMORY_H
#define MEMORY_H

#include "core/types/typedefs.h"

void *ls_malloc(size_t size);
void *ls_realloc(void *ptr, size_t size);
void *ls_calloc(size_t count, size_t size);

void ls_free(void *ptr);

#endif // MEMORY_H