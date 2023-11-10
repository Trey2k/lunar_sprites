#include "core/types/slice.h"

#include "core/debug.h"

#include <stdlib.h>

struct Slice {
	void **data;
	size_t size;
	size_t capacity;
};

Slice *slice_create(size_t inital_size) {
	Slice *slice = ls_malloc(sizeof(Slice));
	slice->data = ls_malloc(sizeof(void *) * inital_size);
	slice->size = 0;
	slice->capacity = inital_size;

	return slice;
}

void slice_destroy(Slice *slice) {
	LS_ASSERT(slice);

	ls_free(slice->data);
	ls_free(slice);
}

void slice_append(Slice *slice, void *element) {
	LS_ASSERT(slice);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(void *) * slice->capacity);
	}

	slice->data[slice->size++] = element;
}

void slice_insert(Slice *slice, size_t index, void *data) {
	LS_ASSERT(slice);
	LS_ASSERT(index <= slice->size);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(void *) * slice->capacity);
	}

	for (size_t i = slice->size; i > index; i--) {
		slice->data[i] = slice->data[i - 1];
	}

	slice->data[index] = data;
	slice->size++;
}

void slice_remove(Slice *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	for (size_t i = index; i < slice->size - 1; i++) {
		slice->data[i] = slice->data[i + 1];
	}

	slice->size--;
}

void slice_remove_range(Slice *slice, size_t index, size_t count) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);
	LS_ASSERT(index + count <= slice->size);

	for (size_t i = index; i < slice->size - count; i++) {
		slice->data[i] = slice->data[i + count];
	}

	slice->size -= count;
}

void slice_clear(Slice *slice) {
	LS_ASSERT(slice);

	slice->size = 0;
}

void slice_sort(Slice *slice, int (*compare)(const void *, const void *)) {
	LS_ASSERT(slice);
	LS_ASSERT(compare);

	qsort(slice->data, slice->size, sizeof(void *), compare);
}

void *slice_get(const Slice *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	return slice->data[index];
}

void *slice_get_last(const Slice *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[slice->size - 1];
}

void *slice_get_first(const Slice *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[0];
}

size_t slice_get_size(const Slice *slice) {
	LS_ASSERT(slice);

	return slice->size;
}

size_t slice_get_capacity(const Slice *slice) {
	LS_ASSERT(slice);

	return slice->capacity;
}
