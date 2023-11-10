#ifndef SLICE_H
#define SLICE_H

#include "core/types/typedefs.h"

typedef bool (*SliceCompareFunc)(const void *a, const void *b);
typedef struct Slice Slice;

Slice *slice_create(size_t inital_size);
void slice_destroy(Slice *slice);

void slice_append(Slice *slice, void *data);
void slice_insert(Slice *slice, size_t index, void *data);
void slice_remove(Slice *slice, size_t index);
void slice_remove_range(Slice *slice, size_t index, size_t count);
void slice_clear(Slice *slice);
void slice_sort(Slice *slice, SliceCompareFunc compare_func);

void *slice_get(const Slice *slice, size_t index);
void *slice_get_last(const Slice *slice);
void *slice_get_first(const Slice *slice);

size_t slice_get_size(const Slice *slice);
size_t slice_get_capacity(const Slice *slice);

#endif // SLICE_H