#ifndef VECTOR2_H
#define VECTOR2_H

#include "core/types/typedefs.h"

typedef struct Vector2 {
	float32 x;
	float32 y;
} Vector2;

typedef struct Vector2i {
	int32 x;
	int32 y;
} Vector2i;

bool vec2_equals(Vector2 a, Vector2 b);

void vec2_copy(Vector2 *dest, Vector2 src);

bool vec2i_equals(Vector2i a, Vector2i b);

void vec2i_copy(Vector2i *dest, Vector2i src);

#endif // VECTOR2_H