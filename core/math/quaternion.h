#ifndef QUANT_H
#define QUANT_H

#include "core/math/matrix.h"
#include "core/math/vector.h"

typedef struct {
	Vector3 vector;
	float32 scalar;
} Quaternion;

#define quat(a_vector, a_scalar) \
	(Quaternion) {               \
		.vector = a_vector,      \
		.scalar = a_scalar       \
	}

#define QUAT_IDENTITY            \
	(Quaternion){                \
		.vector = vec3(0, 0, 0), \
		.scalar = 1.0f           \
	}

Quaternion quat_rotate(const Quaternion quat, float32 angle, const Vector3 axis);
Quaternion quat_multiply(const Quaternion a, const Quaternion b);
Quaternion quat_normalize(const Quaternion quat);
Vector3 quat_multiply_vec3(const Quaternion quat, const Vector3 vector);
Matrix4 quat_rotation_matrix(const Quaternion quat);

#endif // QUANT_H