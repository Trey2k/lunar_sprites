#ifndef slice64_H
#define slice64_H

#include "core/api.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#define SLICE_VAL64(type, val) ((SliceValue64){ .type = val })
#define SLICE_VAL32(type, val) ((SliceValue32){ .type = val })
#define SLICE_VAL128(type, val) ((SliceValue128){ .type.x = val.x, .type.y = val.y })
#define SLICE_VAL8(type, val) ((SliceValue8){ .type = val })

typedef bool (*SliceCompareFunc)(const Variant a, const Variant b);
typedef struct Slice Slice;

// Slice is a dynamic array
// should_free: if true, and the variant is a Object or Resource,
// then it will be freed when the slice is destroyed or the data is removed.
LS_EXPORT Slice *slice_create(size_t inital_size, bool should_free);
LS_EXPORT void slice_destroy(Slice *slice);

LS_EXPORT void slice_append(Slice *slice, Variant data);
LS_EXPORT void slice_append_slice(Slice *slice, const Slice *other);
LS_EXPORT void slice_insert(Slice *slice, size_t index, Variant data);
LS_EXPORT void slice_remove(Slice *slice, size_t index);
LS_EXPORT void slice_remove_range(Slice *slice, size_t index, size_t count);
LS_EXPORT void slice_clear(Slice *slice);
LS_EXPORT void slice_sort(Slice *slice, SliceCompareFunc compare_func);

LS_EXPORT const void *slice_get_data(const Slice *slice);

LS_EXPORT Variant slice_get(const Slice *slice, size_t index);
LS_EXPORT Variant slice_get_last(const Slice *slice);
LS_EXPORT Variant slice_get_first(const Slice *slice);

LS_EXPORT bool slice_is_empty(const Slice *slice);
LS_EXPORT bool slice_contains(const Slice *slice, Variant data);

LS_EXPORT size_t slice_get_size(const Slice *slice);
LS_EXPORT size_t slice_get_capacity(const Slice *slice);

typedef union {
	void *ptr;
	const void *cptr;
	int64 i64;
	uint64 u64;
	float64 f64;
	String str;
} SliceValue64;

typedef bool (*Slice64CompareFunc)(const SliceValue64 a, const SliceValue64 b);
typedef struct Slice64 Slice64;

// Slice64 is a dynamic array
// should_free: if true, the data is assumed to be a pointer,
//      and will be freed when the slice is destroyed or the data is removed.
LS_EXPORT Slice64 *slice64_create(size_t inital_size, bool should_free);
LS_EXPORT void slice64_destroy(Slice64 *slice);

LS_EXPORT void slice64_append(Slice64 *slice, SliceValue64 data);
LS_EXPORT void slice64_append_slice(Slice64 *slice, const Slice64 *other);
LS_EXPORT void slice64_insert(Slice64 *slice, size_t index, SliceValue64 data);
LS_EXPORT void slice64_remove(Slice64 *slice, size_t index);
LS_EXPORT void slice64_remove_range(Slice64 *slice, size_t index, size_t count);
LS_EXPORT void slice64_clear(Slice64 *slice);
LS_EXPORT void slice64_sort(Slice64 *slice, Slice64CompareFunc compare_func);

LS_EXPORT const void *slice64_get_data(const Slice64 *slice);

LS_EXPORT SliceValue64 slice64_get(const Slice64 *slice, size_t index);
LS_EXPORT SliceValue64 slice64_get_last(const Slice64 *slice);
LS_EXPORT SliceValue64 slice64_get_first(const Slice64 *slice);

LS_EXPORT bool slice64_is_empty(const Slice64 *slice);
LS_EXPORT bool slice64_contains(const Slice64 *slice, SliceValue64 data, Slice64CompareFunc compare_func);

LS_EXPORT size_t slice64_get_size(const Slice64 *slice);
LS_EXPORT size_t slice64_get_capacity(const Slice64 *slice);

_FORCE_INLINE_ bool slice64_string_compare(const SliceValue64 a, const SliceValue64 b) {
	return ls_str_equals(a.str, b.str);
}

typedef union {
	bool b;
	int32 i32;
	uint32 u32;
	float32 f32;
} SliceValue32;

typedef bool (*SliceCompareFunc32)(const SliceValue32 a, const SliceValue32 b);
typedef struct Slice32 Slice32;

// Slice32 is a dynamic array
LS_EXPORT Slice32 *slice32_create(size_t inital_size);
LS_EXPORT void slice32_destroy(Slice32 *slice);

LS_EXPORT void slice32_append(Slice32 *slice, SliceValue32 data);
LS_EXPORT void slice32_append_slice(Slice32 *slice, const Slice32 *other);
LS_EXPORT void slice32_insert(Slice32 *slice, size_t index, SliceValue32 data);
LS_EXPORT void slice32_remove(Slice32 *slice, size_t index);
LS_EXPORT void slice32_remove_range(Slice32 *slice, size_t index, size_t count);
LS_EXPORT void slice32_clear(Slice32 *slice);
LS_EXPORT void slice32_sort(Slice32 *slice, SliceCompareFunc32 compare_func);

LS_EXPORT const void *slice32_get_data(const Slice32 *slice);

LS_EXPORT SliceValue32 slice32_get(const Slice32 *slice, size_t index);
LS_EXPORT SliceValue32 slice32_get_last(const Slice32 *slice);
LS_EXPORT SliceValue32 slice32_get_first(const Slice32 *slice);

LS_EXPORT bool slice32_is_empty(const Slice32 *slice);
LS_EXPORT bool slice32_contains(const Slice32 *slice, SliceValue32 data, SliceCompareFunc32 compare_func);

LS_EXPORT size_t slice32_get_size(const Slice32 *slice);
LS_EXPORT size_t slice32_get_capacity(const Slice32 *slice);

typedef union {
	char chr;
	uint8 u8;
} SliceValue8;

typedef bool (*SliceCompareFunc8)(const SliceValue8 a, const SliceValue8 b);
typedef struct Slice8 Slice8;

LS_EXPORT Slice8 *slice8_create(size_t inital_size);
LS_EXPORT void slice8_destroy(Slice8 *slice);

LS_EXPORT void slice8_append(Slice8 *slice, SliceValue8 data);
LS_EXPORT void slice8_append_slice(Slice8 *slice, const Slice8 *other);
LS_EXPORT void slice8_insert(Slice8 *slice, size_t index, SliceValue8 data);
LS_EXPORT void slice8_remove(Slice8 *slice, size_t index);
LS_EXPORT void slice8_remove_range(Slice8 *slice, size_t index, size_t count);
LS_EXPORT void slice8_clear(Slice8 *slice);
LS_EXPORT void slice8_sort(Slice8 *slice, SliceCompareFunc8 compare_func);

LS_EXPORT const void *slice8_get_data(const Slice8 *slice);

LS_EXPORT SliceValue8 slice8_get(const Slice8 *slice, size_t index);
LS_EXPORT SliceValue8 slice8_get_last(const Slice8 *slice);
LS_EXPORT SliceValue8 slice8_get_first(const Slice8 *slice);

LS_EXPORT bool slice8_is_empty(const Slice8 *slice);
LS_EXPORT bool slice8_contains(const Slice8 *slice, SliceValue8 data, SliceCompareFunc8 compare_func);

LS_EXPORT size_t slice8_get_size(const Slice8 *slice);
LS_EXPORT size_t slice8_get_capacity(const Slice8 *slice);

#endif // slice64_H