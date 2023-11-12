#include "core/types/slice.h"

#include "core/debug.h"

#include <stdlib.h>

struct Slice {
	SliceValue *data;
	size_t size;
	size_t capacity;

	bool should_free;
};

Slice *slice_create(size_t inital_size, bool should_free) {
	Slice *slice = ls_malloc(sizeof(Slice));
	slice->data = ls_malloc(sizeof(SliceValue) * inital_size);
	slice->should_free = should_free;
	slice->size = 0;
	slice->capacity = inital_size;

	return slice;
}

void slice_destroy(Slice *slice) {
	LS_ASSERT(slice);

	if (slice->should_free) {
		for (size_t i = 0; i < slice->size; i++) {
			ls_free(slice->data[i].ptr);
		}
	}

	ls_free(slice->data);
	ls_free(slice);
}

void slice_append(Slice *slice, SliceValue element) {
	LS_ASSERT(slice);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue) * slice->capacity);
	}

	slice->data[slice->size++] = element;
}

void slice_insert(Slice *slice, size_t index, SliceValue data) {
	LS_ASSERT(slice);
	LS_ASSERT(index <= slice->size);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue) * slice->capacity);
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

	if (slice->should_free) {
		ls_free(slice->data[index].ptr);
	}

	for (size_t i = index; i < slice->size - 1; i++) {
		slice->data[i] = slice->data[i + 1];
	}

	slice->size--;
}

void slice_remove_range(Slice *slice, size_t index, size_t count) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);
	LS_ASSERT(index + count <= slice->size);

	if (slice->should_free) {
		for (size_t i = index; i < index + count; i++) {
			ls_free(slice->data[i].ptr);
		}
	}

	for (size_t i = index; i < slice->size - count; i++) {
		slice->data[i] = slice->data[i + count];
	}

	slice->size -= count;
}

void slice_clear(Slice *slice) {
	LS_ASSERT(slice);

	if (slice->should_free) {
		for (size_t i = 0; i < slice->size; i++) {
			ls_free(slice->data[i].ptr);
		}
	}

	slice->size = 0;
}

void slice_sort(Slice *slice, int (*compare)(const SliceValue, const SliceValue)) {
	LS_ASSERT(slice);
	LS_ASSERT(compare);

	for (size_t i = 0; i < slice->size; i++) {
		for (size_t j = i + 1; j < slice->size; j++) {
			if (compare(slice->data[i], slice->data[j]) > 0) {
				SliceValue temp = slice->data[i];
				slice->data[i] = slice->data[j];
				slice->data[j] = temp;
			}
		}
	}
}

SliceValue slice_get(const Slice *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	return slice->data[index];
}

SliceValue slice_get_last(const Slice *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[slice->size - 1];
}

SliceValue slice_get_first(const Slice *slice) {
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
