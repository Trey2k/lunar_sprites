#ifndef ARRAY_H
#define ARRAY_H

#include "core/types/typedefs.h"

#include "core/types/bstring.h"

#include "core/api.h"

typedef struct Array Array;

LS_EXPORT Array *array_create(size_t initial_capacity);
LS_EXPORT void array_ref(Array *array);
LS_EXPORT void array_unref(Array *array);

LS_EXPORT void array_append(Array *array, Variant value);
LS_EXPORT void array_append_array(Array *array, const Array *other);
LS_EXPORT void array_insert(Array *array, size_t index, Variant value);
LS_EXPORT void array_remove(Array *array, size_t index);
LS_EXPORT void array_remove_range(Array *array, size_t index, size_t count);
LS_EXPORT void array_clear(Array *array);

LS_EXPORT Variant array_get(const Array *array, size_t index);
LS_EXPORT size_t array_get_size(const Array *array);

LS_EXPORT uint64 array_hash(const Array *array);
LS_EXPORT bool array_equals(const Array *a, const Array *b);

LS_EXPORT BString array_to_string(const Array *array);

#endif // ARRAY_H