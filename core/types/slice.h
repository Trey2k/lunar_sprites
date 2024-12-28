#ifndef SLICE_H
#define SLICE_H

#include "core/api.h"
#include "core/math/vector.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#define SLICE_VAL(type, val) ((SliceValue){ .type = val })
#define SLICE_VAL32(type, val) ((SliceValue32){ .type = val })
#define SLICE_VAL128(type, val) ((SliceValue128){ .type.x = val.x, .type.y = val.y })
#define SLICE_VAL8(type, val) ((SliceValue8){ .type = val })

typedef union {
	void *ptr;
	const void *cptr;
	int64 i64;
	uint64 u64;
	float64 f64;
	String str;
} SliceValue;

typedef bool (*SliceCompareFunc)(const SliceValue a, const SliceValue b);
typedef struct Slice Slice;

// Slice is a dynamic array
// should_free: if true, the data is assumed to be a pointer,
//      and will be freed when the slice is destroyed or the data is removed.
LS_EXPORT Slice *slice_create(size_t inital_size, bool should_free);
LS_EXPORT void slice_destroy(Slice *slice);

LS_EXPORT void slice_append(Slice *slice, SliceValue data);
LS_EXPORT void slice_append_slice(Slice *slice, const Slice *other);
LS_EXPORT void slice_insert(Slice *slice, size_t index, SliceValue data);
LS_EXPORT void slice_remove(Slice *slice, size_t index);
LS_EXPORT void slice_remove_range(Slice *slice, size_t index, size_t count);
LS_EXPORT void slice_clear(Slice *slice);
LS_EXPORT void slice_sort(Slice *slice, SliceCompareFunc compare_func);

LS_EXPORT const void *slice_get_data(const Slice *slice);

LS_EXPORT SliceValue slice_get(const Slice *slice, size_t index);
LS_EXPORT SliceValue slice_get_last(const Slice *slice);
LS_EXPORT SliceValue slice_get_first(const Slice *slice);

LS_EXPORT bool slice_is_empty(const Slice *slice);
LS_EXPORT bool slice_contains(const Slice *slice, SliceValue data, SliceCompareFunc compare_func);

LS_EXPORT size_t slice_get_size(const Slice *slice);
LS_EXPORT size_t slice_get_capacity(const Slice *slice);

_FORCE_INLINE_ bool slice_string_compare(const SliceValue a, const SliceValue b) {
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
	Vector2 v2;
	Vector2i v2i;
} SliceValue128;

typedef bool (*SliceCompareFunc128)(const SliceValue128 a, const SliceValue128 b);
typedef struct Slice128 Slice128;

LS_EXPORT Slice128 *slice128_create(size_t inital_size);
LS_EXPORT void slice128_destroy(Slice128 *slice);

LS_EXPORT void slice128_append(Slice128 *slice, SliceValue128 data);
LS_EXPORT void slice128_append_slice(Slice128 *slice, const Slice128 *other);
LS_EXPORT void slice128_insert(Slice128 *slice, size_t index, SliceValue128 data);
LS_EXPORT void slice128_remove(Slice128 *slice, size_t index);
LS_EXPORT void slice128_remove_range(Slice128 *slice, size_t index, size_t count);
LS_EXPORT void slice128_clear(Slice128 *slice);
LS_EXPORT void slice128_sort(Slice128 *slice, SliceCompareFunc128 compare_func);

LS_EXPORT const void *slice128_get_data(const Slice128 *slice);

LS_EXPORT SliceValue128 slice128_get(const Slice128 *slice, size_t index);
LS_EXPORT SliceValue128 slice128_get_last(const Slice128 *slice);
LS_EXPORT SliceValue128 slice128_get_first(const Slice128 *slice);

LS_EXPORT bool slice128_is_empty(const Slice128 *slice);
LS_EXPORT bool slice128_contains(const Slice128 *slice, SliceValue128 data, SliceCompareFunc128 compare_func);

LS_EXPORT size_t slice128_get_size(const Slice128 *slice);
LS_EXPORT size_t slice128_get_capacity(const Slice128 *slice);

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

#endif // SLICE_H