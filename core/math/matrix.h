#ifndef MATRIX_H
#define MATRIX_H

#include "core/api.h"
#include "core/math/vector.h"
#include "core/types/typedefs.h"

typedef union {
	struct {
		float32 x0;
		float32 y0;
		float32 z0;
		float32 w0;
		float32 x1;
		float32 y1;
		float32 z1;
		float32 w1;
		float32 x2;
		float32 y2;
		float32 z2;
		float32 w2;
		float32 x3;
		float32 y3;
		float32 z3;
		float32 w3;
	};
	float32 mat[16];
} Matrix4;

#define MAT4_IDENTITY   \
	(Matrix4) {         \
		{               \
			.x0 = 1.0f, \
			.y0 = 0.0f, \
			.z0 = 0.0f, \
			.w0 = 0.0f, \
			.x1 = 0.0f, \
			.y1 = 1.0f, \
			.z1 = 0.0f, \
			.w1 = 0.0f, \
			.x2 = 0.0f, \
			.y2 = 0.0f, \
			.z2 = 1.0f, \
			.w2 = 0.0f, \
			.x3 = 0.0f, \
			.y3 = 0.0f, \
			.z3 = 0.0f, \
			.w3 = 1.0f  \
		}               \
	}

LS_EXPORT Matrix4 mat4_ortho(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far);
LS_EXPORT Matrix4 mat4_perspective(float32 fov, float32 aspect, float32 near, float32 far);
LS_EXPORT Matrix4 mat4_translate(const Vector3 translation);
LS_EXPORT Matrix4 mat4_rotate(float32 angle, const Vector3 axis);
LS_EXPORT Matrix4 mat4_scale(const Vector3 scale);
LS_EXPORT Matrix4 mat4_multiply(const Matrix4 a, const Matrix4 b);
LS_EXPORT Vector3 mat4_multiply_vec3(const Matrix4 matrix, const Vector3 vector);
LS_EXPORT Matrix4 mat4_divide(const Matrix4 matrix, float32 value);
LS_EXPORT Matrix4 mat4_add(const Matrix4 a, const Matrix4 b);
LS_EXPORT Matrix4 mat4_subtract(const Matrix4 a, const Matrix4 b);
LS_EXPORT Matrix4 mat4_transpose(const Matrix4 matrix);
LS_EXPORT Matrix4 mat4_invert(const Matrix4 matrix);

LS_EXPORT void mat4_print(const Matrix4 matrix);

LS_EXPORT bool mat4_equals(const Matrix4 a, const Matrix4 b);

#endif // MATRIX_H