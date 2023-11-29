#ifndef VECTOR_H
#define VECTOR_H

#include "core/api.h"
#include "core/types/typedefs.h"

typedef union {
	struct {
		float32 x;
		float32 y;
	};
	float32 vec[2];
} Vector2;

typedef union {
	struct {
		int32 x;
		int32 y;
	};
	int32 vec[2];
} Vector2i;

typedef union {
	struct {
		float32 x;
		float32 y;
		float32 z;
	};
	float32 vec[3];
} Vector3;

typedef union {
	struct {
		int32 x;
		int32 y;
		int32 z;
	};
	int32 vec[3];
} Vector3i;

#define vec2(x, y) ((Vector2){ { x, y } })
#define vec2i(x, y) ((Vector2i){ { x, y } })
#define vec3(x, y, z) ((Vector3){ { x, y, z } })
#define vec3i(x, y, z) ((Vector3i){ { x, y, z } })

LS_EXPORT bool vec2_equals(Vector2 a, Vector2 b);
LS_EXPORT void vec2_copy(Vector2 *dest, Vector2 src);

LS_EXPORT bool vec2i_equals(Vector2i a, Vector2i b);
LS_EXPORT void vec2i_copy(Vector2i *dest, Vector2i src);

LS_EXPORT bool vec3_equals(Vector3 a, Vector3 b);
LS_EXPORT void vec3_copy(Vector3 *dest, Vector3 src);

LS_EXPORT bool vec3i_equals(Vector3i a, Vector3i b);
LS_EXPORT void vec3i_copy(Vector3i *dest, Vector3i src);

#endif // VECTOR_H