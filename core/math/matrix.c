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

Matrix4 mat4_translate(const Matrix4 matrix, const Vector3 translation) {
	Matrix4 translated = MAT4_IDENTITY;

	translated.w0 = translation.x;
	translated.w1 = translation.y;
	translated.w2 = translation.z;

	return mat4_multiply(matrix, translated);
}

static void gram_schmidt_orthonormalize(Matrix4 *mat) {
	Vector3 x = vec3(mat->mat[0], mat->mat[1], mat->mat[2]);
	Vector3 y = vec3(mat->mat[4], mat->mat[5], mat->mat[6]);
	Vector3 z = vec3(mat->mat[8], mat->mat[9], mat->mat[10]);

	// Normalize x
	x = vec3_normalize(x);

	// Orthogonalize y with respect to x
	y = vec3_sub(y, vec3_mul_scalar(x, vec3_dot(x, y)));
	y = vec3_normalize(y);

	// Orthogonalize z with respect to x and y
	z = vec3_sub(z, vec3_mul_scalar(x, vec3_dot(x, z)));
	z = vec3_sub(z, vec3_mul_scalar(y, vec3_dot(y, z)));
	z = vec3_normalize(z);

	// Update the matrix
	mat->mat[0] = x.x;
	mat->mat[1] = x.y;
	mat->mat[2] = x.z;
	mat->mat[4] = y.x;
	mat->mat[5] = y.y;
	mat->mat[6] = y.z;
	mat->mat[8] = z.x;
	mat->mat[9] = z.y;
	mat->mat[10] = z.z;
}

// Angle in radians
Matrix4 mat4_rotate(const Matrix4 matrix, float32 angle, const Vector3 axis) {
	Matrix4 rotated = MAT4_IDENTITY;

	Vector3 n = vec3_normalize(axis);

	float32 c = math_cosf(angle);
	float32 s = math_sinf(angle);
	float32 t = 1.0f - c;

	rotated.mat[0] = t * n.x * n.x + c;
	rotated.mat[1] = t * n.x * n.y + s * n.z;
	rotated.mat[2] = t * n.x * n.z - s * n.y;

	rotated.mat[4] = t * n.x * n.y - s * n.z;
	rotated.mat[5] = t * n.y * n.y + c;
	rotated.mat[6] = t * n.y * n.z + s * n.x;

	rotated.mat[8] = t * n.x * n.z + s * n.y;
	rotated.mat[9] = t * n.y * n.z - s * n.x;
	rotated.mat[10] = t * n.z * n.z + c;

	rotated = mat4_multiply(matrix, rotated);
	gram_schmidt_orthonormalize(&rotated);

	return rotated;
}

Matrix4 mat4_scale(const Matrix4 matrix, const Vector3 scale) {
	Matrix4 scaled = MAT4_IDENTITY;

	scaled.x0 = scale.x;
	scaled.y1 = scale.y;
	scaled.z2 = scale.z;

	return mat4_multiply(matrix, scaled);
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

Vector3 mat4_multiply_vec3(const Matrix4 matrix, const Vector3 vector) {
	Vector3 result;

	result.x = matrix.x0 * vector.x + matrix.y0 * vector.y + matrix.z0 * vector.z + matrix.w0;
	result.y = matrix.x1 * vector.x + matrix.y1 * vector.y + matrix.z1 * vector.z + matrix.w1;
	result.z = matrix.x2 * vector.x + matrix.y2 * vector.y + matrix.z2 * vector.z + matrix.w2;

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