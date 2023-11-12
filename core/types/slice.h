#ifndef SLICE_H
#define SLICE_H

#include "core/types/string.h"
#include "core/types/typedefs.h"

typedef union {
	union {
		void *ptr;
		int64 i64;
		int32 i32;
		uint64 u64;
		uint32 u32;
		uint8 u8;

		float64 f64;
		float32 f32;
		bool b;

		char c;
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
void slice_insert(Slice *slice, size_t index, SliceValue data);
void slice_remove(Slice *slice, size_t index);
void slice_remove_range(Slice *slice, size_t index, size_t count);
void slice_clear(Slice *slice);
void slice_sort(Slice *slice, SliceCompareFunc compare_func);

SliceValue slice_get(const Slice *slice, size_t index);
SliceValue slice_get_last(const Slice *slice);
SliceValue slice_get_first(const Slice *slice);

size_t slice_get_size(const Slice *slice);
size_t slice_get_capacity(const Slice *slice);

#endif // SLICE_H