#include "core/types/array.h"

#include "core/core.h"

// While 2 is on the extreme side of wasted memory, in the long run old blocks will be more likely to be reused.
// https://www.youtube.com/watch?v=GZPqDvG615k
#define ARRAY_GROWTH_FACTOR 2

struct Array {
	Variant *data;
	size_t size;
	size_t capacity;

	size_t ref_count;
};

Array *array_create(size_t initial_capacity) {
	Array *array = ls_malloc(sizeof(Array));
	array->data = ls_malloc(sizeof(Variant) * initial_capacity);
	array->size = 0;
	array->capacity = initial_capacity;
	array->ref_count = 1;

	return array;
}

void array_ref(Array *array) {
	LS_ASSERT(array);

	array->ref_count++;
}

void array_unref(Array *array) {
	LS_ASSERT(array);

	array->ref_count--;

	if (array->ref_count != 0) {
		return;
	}

	for (size_t i = 0; i < array->size; i++) {
		variant_unref(array->data[i]);
	}

	ls_free(array->data);
	ls_free(array);
}

void array_append(Array *array, Variant value) {
	LS_ASSERT(array);

	if (array->size >= array->capacity) {
		array->capacity *= ARRAY_GROWTH_FACTOR;
		array->data = ls_realloc(array->data, sizeof(Variant) * array->capacity);
	}
	variant_ref(value);
	array->data[array->size++] = value;
}

void array_append_array(Array *array, const Array *other) {
	LS_ASSERT(array);
	LS_ASSERT(other);

	for (size_t i = 0; i < other->size; i++) {
		variant_ref(other->data[i]);
		array_append(array, other->data[i]);
	}
}

void array_insert(Array *array, size_t index, Variant value) {
	LS_ASSERT(array);
	LS_ASSERT(index <= array->size);

	if (array->size >= array->capacity) {
		array->capacity *= ARRAY_GROWTH_FACTOR;
		array->data = ls_realloc(array->data, sizeof(Variant) * array->capacity);
	}

	for (size_t i = array->size; i > index; i--) {
		array->data[i] = array->data[i - 1];
	}

	variant_ref(value);
	variant_unref(array->data[index]);

	array->data[index] = value;
	array->size++;
}

void array_remove(Array *array, size_t index) {
	LS_ASSERT(array);
	LS_ASSERT(index < array->size);

	for (size_t i = index; i < array->size - 1; i++) {
		variant_unref(array->data[i]);
		array->data[i] = array->data[i + 1];
	}

	array->size--;
}

void array_remove_range(Array *array, size_t index, size_t count) {
	LS_ASSERT(array);
	LS_ASSERT(index + count <= array->size);

	for (size_t i = index; i < array->size - count; i++) {
		variant_unref(array->data[i]);
		array->data[i] = array->data[i + count];
	}

	array->size -= count;
}

void array_clear(Array *array) {
	LS_ASSERT(array);

	for (size_t i = 0; i < array->size; i++) {
		variant_unref(array->data[i]);
	}

	array->size = 0;
}

Variant array_get(const Array *array, size_t index) {
	LS_ASSERT(array);
	LS_ASSERT(index < array->size);

	return array->data[index];
}

size_t array_get_size(const Array *array) {
	LS_ASSERT(array);

	return array->size;
}

uint64 array_hash(const Array *array) {
	LS_ASSERT(array);

	uint64 hash = 0;
	for (size_t i = 0; i < array->size; i++) {
		hash += variant_hash(array->data[i]);
	}

	return hash;
}

bool array_equals(const Array *a, const Array *b) {
	LS_ASSERT(a);
	LS_ASSERT(b);

	if (a->size != b->size) {
		return false;
	}

	for (size_t i = 0; i < a->size; i++) {
		if (!variant_equals(a->data[i], b->data[i])) {
			return false;
		}
	}

	return true;
}

BString array_to_string(const Array *array) {
	LS_ASSERT(array);

	BString str = BSTRING_CONST("Array(");
	for (size_t i = 0; i < array->size; i++) {
		Variant val = array->data[i];

		if (val.type == VARIANT_TYPE_ARRAY && val.ARRAY == array) {
			bstring_append_cstr(&str, "Array(SELF_REFERENCE)", 21);
		} else {
			BString val_str = variant_to_string(val);
			bstring_append(&str, val_str);
			bstring_unref(val_str);
		}

		if (i < array->size - 1) {
			bstring_append_cstr(&str, ", ", 2);
		}
	}
	bstring_append_char(&str, ')');

	return str;
}