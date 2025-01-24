#include "quaternion.h"

#include "core/math/math.h"
#include "core/math/matrix.h"
#include "core/math/vector.h"

Quaternion quat_rotate(const Quaternion quat, float32 angle, const Vector3 axis) {
	Quaternion rotated = QUAT_IDENTITY;

	Vector3 n = vec3_normalize(axis);

	float32 s = math_sinf(angle / 2.0f);
	float32 c = math_cosf(angle / 2.0f);

	rotated.vector = vec3_mul_scalar(n, s);
	rotated.scalar = c;

	return quat_multiply(quat, rotated);
}

Quaternion quat_multiply(const Quaternion a, const Quaternion b) {
	Quaternion result = QUAT_IDENTITY;

	result.vector = vec3_add(
			vec3_add(
					vec3_mul_scalar(a.vector, b.scalar),
					vec3_mul_scalar(b.vector, a.scalar)),
			vec3_cross(a.vector, b.vector));

	result.scalar = a.scalar * b.scalar - vec3_dot(a.vector, b.vector);

	return result;
}

Quaternion quat_normalize(const Quaternion quat) {
	float32 length = math_sqrtf(quat.scalar * quat.scalar + quat.vector.x * quat.vector.x + quat.vector.y * quat.vector.y + quat.vector.z * quat.vector.z);

	Quaternion result = QUAT_IDENTITY;

	if (length > 0.0f) {
		float32 invLength = 1.0f / length;
		result.scalar = quat.scalar * invLength;
		result.vector = vec3_mul_scalar(quat.vector, invLength);
	}

	return result;
}

Vector3 quat_multiply_vec3(const Quaternion quat, const Vector3 vector) {
	Quaternion a = quat(vector, 0.0f);
	Quaternion a_inv = quat(vec3_negate(quat.vector), quat.scalar);
	Quaternion b = quat_multiply(quat_multiply(quat, a), a_inv);

	return b.vector;
}

Matrix4 quat_rotation_matrix(const Quaternion quat) {
	Matrix4 result = MAT4_IDENTITY;

	float32 x = quat.vector.x;
	float32 y = quat.vector.y;
	float32 z = quat.vector.z;
	float32 w = quat.scalar;

	float32 x2 = x + x;
	float32 y2 = y + y;
	float32 z2 = z + z;

	float32 xx = x * x2;
	float32 xy = x * y2;
	float32 xz = x * z2;
	float32 yy = y * y2;
	float32 yz = y * z2;
	float32 zz = z * z2;
	float32 wx = w * x2;
	float32 wy = w * y2;
	float32 wz = w * z2;

	result.mat[0] = 1.0f - (yy + zz);
	result.mat[1] = xy + wz;
	result.mat[2] = xz - wy;

	result.mat[4] = xy - wz;
	result.mat[5] = 1.0f - (xx + zz);
	result.mat[6] = yz + wx;

	result.mat[8] = xz + wy;
	result.mat[9] = yz - wx;
	result.mat[10] = 1.0f - (xx + yy);

	return result;
}

Vector3 quat_rotate_vector(const Quaternion quat, const Vector3 vector) {
	Quaternion q = quat_multiply(quat, quat_multiply(quat_rotate(quat, math_deg_to_rad(180), vec3(0, 1, 0)), quat_rotate(quat, math_deg_to_rad(180), vec3(0, 0, 1))));

	Quaternion q_inv = quat_rotate(quat, math_deg_to_rad(-180), vec3(0, 0, 1));

	Quaternion rotated = quat_multiply(quat_multiply(q, quat), q_inv);

	return vec3(rotated.vector.x, rotated.vector.y, rotated.vector.z);
}