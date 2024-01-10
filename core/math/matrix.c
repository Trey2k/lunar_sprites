#include "core/math/matrix.h"
#include "core/log.h"
#include "core/math/math.h"

Matrix4 mat4_ortho(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far) {
	Matrix4 result = MAT4_IDENTITY;

	result.x0 = 2.0f / (right - left);
	result.y1 = 2.0f / (top - bottom);
	result.z2 = -2.0f / (far - near);
	result.w0 = -(right + left) / (right - left);
	result.w1 = -(top + bottom) / (top - bottom);
	result.w2 = -(far + near) / (far - near);

	return result;
}

Matrix4 mat4_perspective(float32 fov, float32 aspect, float32 near, float32 far) {
	Matrix4 result = MAT4_IDENTITY;

	float32 tan_half_fov = math_tanf(fov / 2.0f);

	result.x0 = 1.0f / (aspect * tan_half_fov);
	result.y1 = 1.0f / tan_half_fov;
	result.z2 = -(far + near) / (far - near);
	result.z3 = -1.0f;
	result.w2 = -(2.0f * far * near) / (far - near);

	return result;
}

Matrix4 mat4_translate(const Vector3 translation) {
	Matrix4 result = MAT4_IDENTITY;

	result.x3 = translation.x;
	result.y3 = translation.y;
	result.z3 = translation.z;

	return result;
}

Matrix4 mat4_rotate(float32 angle, const Vector3 axis) {
	Matrix4 result = MAT4_IDENTITY;

	float32 r = angle * (PI / 180.0f);
	float32 c = math_cosf(r);
	float32 s = math_sinf(r);
	float32 omc = 1.0f - c;

	result.x0 = axis.x * axis.x * omc + c;
	result.y0 = axis.x * axis.y * omc + axis.z * s;
	result.z0 = axis.x * axis.z * omc - axis.y * s;

	result.x1 = axis.x * axis.y * omc - axis.z * s;
	result.y1 = axis.y * axis.y * omc + c;
	result.z1 = axis.y * axis.z * omc + axis.x * s;

	result.x2 = axis.x * axis.z * omc + axis.y * s;
	result.y2 = axis.y * axis.z * omc - axis.x * s;
	result.z2 = axis.z * axis.z * omc + c;

	return result;
}

Matrix4 mat4_scale(const Vector3 scale) {
	Matrix4 result = MAT4_IDENTITY;

	result.x0 = scale.x;
	result.y1 = scale.y;
	result.z2 = scale.z;

	return result;
}

Matrix4 mat4_multiply(const Matrix4 a, const Matrix4 b) {
	Matrix4 result;

	for (int32 i = 0; i < 4; i++) {
		for (int32 j = 0; j < 4; j++) {
			result.mat[i * 4 + j] = 0.0f;
			for (int32 k = 0; k < 4; k++) {
				result.mat[i * 4 + j] += a.mat[i * 4 + k] * b.mat[k * 4 + j];
			}
		}
	}

	return result;
}

Matrix4 mat4_divide(const Matrix4 matrix, float32 value) {
	Matrix4 result = MAT4_IDENTITY;

	for (int32 i = 0; i < 16; i++) {
		result.mat[i] = matrix.mat[i] / value;
	}

	return result;
}

Matrix4 mat4_add(const Matrix4 a, const Matrix4 b) {
	Matrix4 result = MAT4_IDENTITY;

	for (int32 i = 0; i < 16; i++) {
		result.mat[i] = a.mat[i] + b.mat[i];
	}

	return result;
}

Matrix4 mat4_subtract(const Matrix4 a, const Matrix4 b) {
	Matrix4 result = MAT4_IDENTITY;

	for (int32 i = 0; i < 16; i++) {
		result.mat[i] = a.mat[i] - b.mat[i];
	}

	return result;
}

Matrix4 mat4_transpose(const Matrix4 matrix) {
	Matrix4 result = MAT4_IDENTITY;

	for (int32 i = 0; i < 4; i++) {
		for (int32 j = 0; j < 4; j++) {
			result.mat[i * 4 + j] = matrix.mat[j * 4 + i];
		}
	}

	return result;
}

Matrix4 mat4_invert(const Matrix4 matrix) {
	Matrix4 result = MAT4_IDENTITY;

	for (int32 i = 0; i < 4; i++) {
		for (int32 j = 0; j < 4; j++) {
			result.mat[i * 4 + j] = matrix.mat[j * 4 + i];
		}
	}

	return result;
}

void mat4_print(const Matrix4 matrix) {
	ls_printf("Matrix4: \n");
	ls_printf("%f, %f, %f, %f\n", matrix.x0, matrix.y0, matrix.z0, matrix.w0);
	ls_printf("%f, %f, %f, %f\n", matrix.x1, matrix.y1, matrix.z1, matrix.w1);
	ls_printf("%f, %f, %f, %f\n", matrix.x2, matrix.y2, matrix.z2, matrix.w2);
	ls_printf("%f, %f, %f, %f\n", matrix.x3, matrix.y3, matrix.z3, matrix.w3);
}

bool mat4_equals(const Matrix4 a, const Matrix4 b) {
	for (int32 i = 0; i < 16; i++) {
		if (a.mat[i] != b.mat[i]) {
			return false;
		}
	}

	return true;
}