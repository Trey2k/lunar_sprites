#ifndef VECTOR3_H
#define VECTOR3_H

#include "core/api.h"
#include "core/types/typedefs.h"

typedef struct {
	float32 x;
	float32 y;
	float32 z;
} Vector3;

typedef struct {
	int32 x;
	int32 y;
	int32 z;
} Vector3i;

LS_EXPORT bool vec3_equals(Vector3 a, Vector3 b);

LS_EXPORT void vec3_copy(Vector3 *dest, Vector3 src);

LS_EXPORT bool vec3i_equals(Vector3i a, Vector3i b);

LS_EXPORT void vec3i_copy(Vector3i *dest, Vector3i src);

#endif // VECTOR3_H