#ifndef SLICE_H
#define SLICE_H

#include "core/types/string.h"
#include "core/types/typedefs.h"

#define SLICE_VAL(type, val) ((SliceValue){ .type = val })
#define SLICE_VAL32(type, val) ((SliceValue32){ .type = val })

typedef union {
	union {
		void *ptr;
		int64 i64;
		uint64 u64;
		float64 f64;
		String str;
	};
} SliceValue;

typedef bool (*SliceCompareFunc)(const SliceValue a, const SliceValue b);
typedef struct Slice Slice;

// Slice is a dynamic array
// should_free: if true, the data is assumed to be a pointer,
//      and will be freed when the slice is destroyed or the data is removed.
Slice *slice_create(size_t inital_size, bool should_free);
void slice_destroy(Slice *slice);

void slice_append(Slice *slice, SliceValue data);
void slice_append_slice(Slice *slice, const Slice *other);
void slice_insert(Slice *slice, size_t index, SliceValue data);
void slice_remove(Slice *slice, size_t index);
void slice_remove_range(Slice *slice, size_t index, size_t count);
void slice_clear(Slice *slice);
void slice_sort(Slice *slice, SliceCompareFunc compare_func);

const void *slice_get_data(const Slice *slice);

SliceValue slice_get(const Slice *slice, size_t index);
SliceValue slice_get_last(const Slice *slice);
SliceValue slice_get_first(const Slice *slice);

bool slice_is_empty(const Slice *slice);
bool slice_contains(const Slice *slice, SliceValue data, SliceCompareFunc compare_func);

size_t slice_get_size(const Slice *slice);
size_t slice_get_capacity(const Slice *slice);

_FORCE_INLINE_ bool slice_string_compare(const SliceValue a, const SliceValue b) {
	return ls_str_equals(a.str, b.str);
}

typedef union {
	union {
		bool b;
		int32 i32;
		uint32 u32;
		float32 f32;
	};
} SliceValue32;

typedef bool (*SliceCompareFunc32)(const SliceValue32 a, const SliceValue32 b);
typedef struct Slice32 Slice32;

// Slice32 is a dynamic array
Slice32 *slice32_create(size_t inital_size);
void slice32_destroy(Slice32 *slice);

void slice32_append(Slice32 *slice, SliceValue32 data);
void slice32_append_slice(Slice32 *slice, const Slice32 *other);
void slice32_insert(Slice32 *slice, size_t index, SliceValue32 data);
void slice32_remove(Slice32 *slice, size_t index);
void slice32_remove_range(Slice32 *slice, size_t index, size_t count);
void slice32_clear(Slice32 *slice);
void slice32_sort(Slice32 *slice, SliceCompareFunc32 compare_func);

const void *slice32_get_data(const Slice32 *slice);

SliceValue32 slice32_get(const Slice32 *slice, size_t index);
SliceValue32 slice32_get_last(const Slice32 *slice);
SliceValue32 slice32_get_first(const Slice32 *slice);

bool slice32_is_empty(const Slice32 *slice);
bool slice32_contains(const Slice32 *slice, SliceValue32 data, SliceCompareFunc32 compare_func);

size_t slice32_get_size(const Slice32 *slice);
size_t slice32_get_capacity(const Slice32 *slice);

#endif // SLICE_H