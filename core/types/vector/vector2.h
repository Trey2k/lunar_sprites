#ifndef VECTOR2_H
#define VECTOR2_H

#include "core/api.h"
#include "core/types/typedefs.h"

typedef struct {
	float32 x;
	float32 y;
} Vector2;

typedef struct {
	int32 x;
	int32 y;
} Vector2i;

LS_EXPORT bool vec2_equals(Vector2 a, Vector2 b);

LS_EXPORT void vec2_copy(Vector2 *dest, Vector2 src);

LS_EXPORT bool vec2i_equals(Vector2i a, Vector2i b);

LS_EXPORT void vec2i_copy(Vector2i *dest, Vector2i src);

#endif // VECTOR2_H