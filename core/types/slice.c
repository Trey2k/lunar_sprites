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

void slice_append_slice(Slice *slice, const Slice *other) {
	LS_ASSERT(slice);
	LS_ASSERT(other);

	for (size_t i = 0; i < other->size; i++) {
		slice_append(slice, other->data[i]);
	}
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

void slice_sort(Slice *slice, SliceCompareFunc compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		for (size_t j = i + 1; j < slice->size; j++) {
			if (compare_func(slice->data[i], slice->data[j])) {
				SliceValue temp = slice->data[i];
				slice->data[i] = slice->data[j];
				slice->data[j] = temp;
			}
		}
	}
}

const void *slice_get_data(const Slice *slice) {
	LS_ASSERT(slice);

	return slice->data;
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

bool slice_is_empty(const Slice *slice) {
	LS_ASSERT(slice);

	return slice->size == 0;
}

bool slice_contains(const Slice *slice, SliceValue data, SliceCompareFunc compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		if (compare_func(slice->data[i], data)) {
			return true;
		}
	}

	return false;
}

size_t slice_get_size(const Slice *slice) {
	LS_ASSERT(slice);

	return slice->size;
}

size_t slice_get_capacity(const Slice *slice) {
	LS_ASSERT(slice);

	return slice->capacity;
}

struct Slice32 {
	SliceValue32 *data;
	size_t size;
	size_t capacity;
};

Slice32 *slice32_create(size_t inital_size) {
	Slice32 *slice = ls_malloc(sizeof(Slice32));

	slice->data = ls_malloc(sizeof(SliceValue32) * inital_size);
	slice->size = 0;
	slice->capacity = inital_size;

	return slice;
}

void slice32_destroy(Slice32 *slice) {
	LS_ASSERT(slice);

	ls_free(slice->data);
	ls_free(slice);
}

void slice32_append(Slice32 *slice, SliceValue32 element) {
	LS_ASSERT(slice);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue32) * slice->capacity);
	}

	slice->data[slice->size++] = element;
}

void slice32_append_slice(Slice32 *slice, const Slice32 *other) {
	LS_ASSERT(slice);
	LS_ASSERT(other);

	for (size_t i = 0; i < other->size; i++) {
		slice32_append(slice, other->data[i]);
	}
}

void slice32_insert(Slice32 *slice, size_t index, SliceValue32 data) {
	LS_ASSERT(slice);
	LS_ASSERT(index <= slice->size);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue32) * slice->capacity);
	}

	for (size_t i = slice->size; i > index; i--) {
		slice->data[i] = slice->data[i - 1];
	}

	slice->data[index] = data;
	slice->size++;
}

void slice32_remove(Slice32 *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	for (size_t i = index; i < slice->size - 1; i++) {
		slice->data[i] = slice->data[i + 1];
	}

	slice->size--;
}

void slice32_remove_range(Slice32 *slice, size_t index, size_t count) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);
	LS_ASSERT(index + count <= slice->size);

	for (size_t i = index; i < slice->size - count; i++) {
		slice->data[i] = slice->data[i + count];
	}

	slice->size -= count;
}

void slice32_clear(Slice32 *slice) {
	LS_ASSERT(slice);

	slice->size = 0;
}

void slice32_sort(Slice32 *slice, SliceCompareFunc32 compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		for (size_t j = i + 1; j < slice->size; j++) {
			if (compare_func(slice->data[i], slice->data[j])) {
				SliceValue32 temp = slice->data[i];
				slice->data[i] = slice->data[j];
				slice->data[j] = temp;
			}
		}
	}
}

const void *slice32_get_data(const Slice32 *slice) {
	LS_ASSERT(slice);

	return slice->data;
}

SliceValue32 slice32_get(const Slice32 *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	return slice->data[index];
}

SliceValue32 slice32_get_last(const Slice32 *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[slice->size - 1];
}

SliceValue32 slice32_get_first(const Slice32 *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[0];
}

bool slice32_is_empty(const Slice32 *slice) {
	LS_ASSERT(slice);

	return slice->size == 0;
}

bool slice32_contains(const Slice32 *slice, SliceValue32 data, SliceCompareFunc32 compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		if (compare_func(slice->data[i], data)) {
			return true;
		}
	}

	return false;
}

size_t slice32_get_size(const Slice32 *slice) {
	LS_ASSERT(slice);

	return slice->size;
}

size_t slice32_get_capacity(const Slice32 *slice) {
	LS_ASSERT(slice);

	return slice->capacity;
}

struct Slice128 {
	SliceValue128 *data;
	size_t size;
	size_t capacity;
};

Slice128 *slice128_create(size_t inital_size) {
	Slice128 *slice = ls_malloc(sizeof(Slice128));

	slice->data = ls_malloc(sizeof(SliceValue128) * inital_size);
	slice->size = 0;
	slice->capacity = inital_size;

	return slice;
}

void slice128_destroy(Slice128 *slice) {
	LS_ASSERT(slice);

	ls_free(slice->data);
	ls_free(slice);
}

void slice128_append(Slice128 *slice, SliceValue128 element) {
	LS_ASSERT(slice);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue128) * slice->capacity);
	}

	slice->data[slice->size++] = element;
}

void slice128_append_slice(Slice128 *slice, const Slice128 *other) {
	LS_ASSERT(slice);
	LS_ASSERT(other);

	for (size_t i = 0; i < other->size; i++) {
		slice128_append(slice, other->data[i]);
	}
}

void slice128_insert(Slice128 *slice, size_t index, SliceValue128 data) {
	LS_ASSERT(slice);
	LS_ASSERT(index <= slice->size);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue128) * slice->capacity);
	}

	for (size_t i = slice->size; i > index; i--) {
		slice->data[i] = slice->data[i - 1];
	}

	slice->data[index] = data;
	slice->size++;
}

void slice128_remove(Slice128 *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	for (size_t i = index; i < slice->size - 1; i++) {
		slice->data[i] = slice->data[i + 1];
	}

	slice->size--;
}

void slice128_remove_range(Slice128 *slice, size_t index, size_t count) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);
	LS_ASSERT(index + count <= slice->size);

	for (size_t i = index; i < slice->size - count; i++) {
		slice->data[i] = slice->data[i + count];
	}

	slice->size -= count;
}

void slice128_clear(Slice128 *slice) {
	LS_ASSERT(slice);

	slice->size = 0;
}

void slice128_sort(Slice128 *slice, SliceCompareFunc128 compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		for (size_t j = i + 1; j < slice->size; j++) {
			if (compare_func(slice->data[i], slice->data[j])) {
				SliceValue128 temp = slice->data[i];
				slice->data[i] = slice->data[j];
				slice->data[j] = temp;
			}
		}
	}
}

const void *slice128_get_data(const Slice128 *slice) {
	LS_ASSERT(slice);

	return slice->data;
}

SliceValue128 slice128_get(const Slice128 *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	return slice->data[index];
}

SliceValue128 slice128_get_last(const Slice128 *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[slice->size - 1];
}

SliceValue128 slice128_get_first(const Slice128 *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[0];
}

bool slice128_is_empty(const Slice128 *slice) {
	LS_ASSERT(slice);

	return slice->size == 0;
}

bool slice128_contains(const Slice128 *slice, SliceValue128 data, SliceCompareFunc128 compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		if (compare_func(slice->data[i], data)) {
			return true;
		}
	}

	return false;
}

size_t slice128_get_size(const Slice128 *slice) {
	LS_ASSERT(slice);

	return slice->size;
}

size_t slice128_get_capacity(const Slice128 *slice) {
	LS_ASSERT(slice);

	return slice->capacity;
}

struct Slice8 {
	SliceValue8 *data;
	size_t size;
	size_t capacity;
};

Slice8 *slice8_create(size_t inital_size) {
	Slice8 *slice = ls_malloc(sizeof(Slice8));

	slice->data = ls_malloc(sizeof(SliceValue8) * inital_size);
	slice->size = 0;
	slice->capacity = inital_size;

	return slice;
}

void slice8_destroy(Slice8 *slice) {
	LS_ASSERT(slice);

	ls_free(slice->data);
	ls_free(slice);
}

void slice8_append(Slice8 *slice, SliceValue8 element) {
	LS_ASSERT(slice);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue8) * slice->capacity);
	}

	slice->data[slice->size++] = element;
}

void slice8_append_slice(Slice8 *slice, const Slice8 *other) {
	LS_ASSERT(slice);
	LS_ASSERT(other);

	for (size_t i = 0; i < other->size; i++) {
		slice8_append(slice, other->data[i]);
	}
}

void slice8_insert(Slice8 *slice, size_t index, SliceValue8 data) {
	LS_ASSERT(slice);
	LS_ASSERT(index <= slice->size);

	if (slice->size == slice->capacity) {
		slice->capacity *= 2;
		slice->data = ls_realloc(slice->data, sizeof(SliceValue8) * slice->capacity);
	}

	for (size_t i = slice->size; i > index; i--) {
		slice->data[i] = slice->data[i - 1];
	}

	slice->data[index] = data;
	slice->size++;
}

void slice8_remove(Slice8 *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	for (size_t i = index; i < slice->size - 1; i++) {
		slice->data[i] = slice->data[i + 1];
	}

	slice->size--;
}

void slice8_remove_range(Slice8 *slice, size_t index, size_t count) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);
	LS_ASSERT(index + count <= slice->size);

	for (size_t i = index; i < slice->size - count; i++) {
		slice->data[i] = slice->data[i + count];
	}

	slice->size -= count;
}

void slice8_clear(Slice8 *slice) {
	LS_ASSERT(slice);

	slice->size = 0;
}

void slice8_sort(Slice8 *slice, SliceCompareFunc8 compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		for (size_t j = i + 1; j < slice->size; j++) {
			if (compare_func(slice->data[i], slice->data[j])) {
				SliceValue8 temp = slice->data[i];
				slice->data[i] = slice->data[j];
				slice->data[j] = temp;
			}
		}
	}
}

const void *slice8_get_data(const Slice8 *slice) {
	LS_ASSERT(slice);

	return slice->data;
}

SliceValue8 slice8_get(const Slice8 *slice, size_t index) {
	LS_ASSERT(slice);
	LS_ASSERT(index < slice->size);

	return slice->data[index];
}

SliceValue8 slice8_get_last(const Slice8 *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[slice->size - 1];
}

SliceValue8 slice8_get_first(const Slice8 *slice) {
	LS_ASSERT(slice);
	LS_ASSERT(slice->size > 0);

	return slice->data[0];
}

bool slice8_is_empty(const Slice8 *slice) {
	LS_ASSERT(slice);

	return slice->size == 0;
}

bool slice8_contains(const Slice8 *slice, SliceValue8 data, SliceCompareFunc8 compare_func) {
	LS_ASSERT(slice);
	LS_ASSERT(compare_func);

	for (size_t i = 0; i < slice->size; i++) {
		if (compare_func(slice->data[i], data)) {
			return true;
		}
	}

	return false;
}

size_t slice8_get_size(const Slice8 *slice) {
	LS_ASSERT(slice);

	return slice->size;
}

size_t slice8_get_capacity(const Slice8 *slice) {
	LS_ASSERT(slice);

	return slice->capacity;
}