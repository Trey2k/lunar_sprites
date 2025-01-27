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
		uint32 x;
		uint32 y;
	};
	uint32 vec[2];
} Vector2u;

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

typedef union {
	struct {
		uint32 x;
		uint32 y;
		uint32 z;
	};
	uint32 vec[3];
} Vector3u;

#define vec2(x, y) ((Vector2){ { x, y } })
#define vec2i(x, y) ((Vector2i){ { x, y } })
#define vec2u(x, y) ((Vector2u){ { x, y } })
#define vec3(x, y, z) ((Vector3){ { x, y, z } })
#define vec3i(x, y, z) ((Vector3i){ { x, y, z } })
#define vec3u(x, y, z) ((Vector3u){ { x, y, z } })

#define VEC2_UP vec2(0.0f, 1.0f)
#define VEC2_DOWN vec2(0.0f, -1.0f)
#define VEC2_LEFT vec2(-1.0f, 0.0f)
#define VEC2_RIGHT vec2(1.0f, 0.0f)

#define VEC2I_UP vec2i(0, 1)
#define VEC2I_DOWN vec2i(0, -1)
#define VEC2I_LEFT vec2i(-1, 0)
#define VEC2I_RIGHT vec2i(1, 0)

#define VEC3_UP vec3(0.0f, 1.0f, 0.0f)
#define VEC3_DOWN vec3(0.0f, -1.0f, 0.0f)
#define VEC3_LEFT vec3(-1.0f, 0.0f, 0.0f)
#define VEC3_RIGHT vec3(1.0f, 0.0f, 0.0f)

#define VEC3I_UP vec3i(0, 1, 0)
#define VEC3I_DOWN vec3i(0, -1, 0)
#define VEC3I_LEFT vec3i(-1, 0, 0)
#define VEC3I_RIGHT vec3i(1, 0, 0)

LS_EXPORT bool vec2_equals(Vector2 a, Vector2 b);
LS_EXPORT void vec2_copy(Vector2 *dest, Vector2 src);
LS_EXPORT Vector2 vec2_add(Vector2 a, Vector2 b);
LS_EXPORT Vector2 vec2_sub(Vector2 a, Vector2 b);
LS_EXPORT Vector2 vec2_mul(Vector2 a, Vector2 b);
LS_EXPORT Vector2 vec2_div(Vector2 a, Vector2 b);
LS_EXPORT Vector2 vec2_negate(Vector2 a);
LS_EXPORT uint64 vec2_hash(Vector2 a);

LS_EXPORT bool vec2i_equals(Vector2i a, Vector2i b);
LS_EXPORT void vec2i_copy(Vector2i *dest, Vector2i src);
LS_EXPORT Vector2i vec2i_add(Vector2i a, Vector2i b);
LS_EXPORT Vector2i vec2i_sub(Vector2i a, Vector2i b);
LS_EXPORT Vector2i vec2i_mul(Vector2i a, Vector2i b);
LS_EXPORT Vector2i vec2i_div(Vector2i a, Vector2i b);
LS_EXPORT Vector2i vec2i_negate(Vector2i a);
LS_EXPORT uint64 vec2i_hash(Vector2i a);

LS_EXPORT bool vec2u_equals(Vector2u a, Vector2u b);
LS_EXPORT void vec2u_copy(Vector2u *dest, Vector2u src);
LS_EXPORT Vector2u vec2u_add(Vector2u a, Vector2u b);
LS_EXPORT Vector2u vec2u_sub(Vector2u a, Vector2u b);
LS_EXPORT Vector2u vec2u_mul(Vector2u a, Vector2u b);
LS_EXPORT Vector2u vec2u_div(Vector2u a, Vector2u b);
LS_EXPORT Vector2u vec2u_negate(Vector2u a);

LS_EXPORT bool vec3_equals(Vector3 a, Vector3 b);
LS_EXPORT void vec3_copy(Vector3 *dest, Vector3 src);
LS_EXPORT Vector3 vec3_add(Vector3 a, Vector3 b);
LS_EXPORT Vector3 vec3_sub(Vector3 a, Vector3 b);
LS_EXPORT Vector3 vec3_mul(Vector3 a, Vector3 b);
LS_EXPORT Vector3 vec3_mul_scalar(Vector3 a, float32 scalar);
LS_EXPORT Vector3 vec3_div(Vector3 a, Vector3 b);
LS_EXPORT Vector3 vec3_negate(Vector3 a);
LS_EXPORT Vector3 vec3_normalize(Vector3 a);
LS_EXPORT float32 vec3_dot(Vector3 a, Vector3 b);
LS_EXPORT Vector3 vec3_cross(Vector3 a, Vector3 b);
LS_EXPORT uint64 vec3_hash(Vector3 a);

LS_EXPORT bool vec3i_equals(Vector3i a, Vector3i b);
LS_EXPORT void vec3i_copy(Vector3i *dest, Vector3i src);
LS_EXPORT Vector3i vec3i_add(Vector3i a, Vector3i b);
LS_EXPORT Vector3i vec3i_sub(Vector3i a, Vector3i b);
LS_EXPORT Vector3i vec3i_mul(Vector3i a, Vector3i b);
LS_EXPORT Vector3i vec3i_div(Vector3i a, Vector3i b);
LS_EXPORT Vector3i vec3i_negate(Vector3i a);
LS_EXPORT uint64 vec3i_hash(Vector3i a);

LS_EXPORT bool vec3u_equals(Vector3u a, Vector3u b);
LS_EXPORT void vec3u_copy(Vector3u *dest, Vector3u src);
LS_EXPORT Vector3u vec3u_add(Vector3u a, Vector3u b);
LS_EXPORT Vector3u vec3u_sub(Vector3u a, Vector3u b);
LS_EXPORT Vector3u vec3u_mul(Vector3u a, Vector3u b);
LS_EXPORT Vector3u vec3u_div(Vector3u a, Vector3u b);
LS_EXPORT Vector3u vec3u_negate(Vector3u a);

#endif // VECTOR_H