#include "core/math/vector.h"
#include "core/math/math.h"

bool vec2_equals(Vector2 a, Vector2 b) {
	return a.x == b.x && a.y == b.y;
}

void vec2_copy(Vector2 *dest, Vector2 src) {
	dest->x = src.x;
	dest->y = src.y;
}

Vector2 vec2_add(Vector2 a, Vector2 b) {
	return vec2(a.x + b.x, a.y + b.y);
}

Vector2 vec2_sub(Vector2 a, Vector2 b) {
	return vec2(a.x - b.x, a.y - b.y);
}

Vector2 vec2_mul(Vector2 a, Vector2 b) {
	return vec2(a.x * b.x, a.y * b.y);
}

Vector2 vec2_div(Vector2 a, Vector2 b) {
	return vec2(a.x / b.x, a.y / b.y);
}

Vector2 vec2_negate(Vector2 a) {
	return vec2(-a.x, -a.y);
}

uint64 vec2_hash(Vector2 a) {
	// 2 32 bit values can fit directly into a 64 bit value.
	union {
		Vector2 v;
		uint64 hash;
	} shim;
	shim.v = a;
	return shim.hash;
}

bool vec2i_equals(Vector2i a, Vector2i b) {
	return a.x == b.x && a.y == b.y;
}

void vec2i_copy(Vector2i *dest, Vector2i src) {
	dest->x = src.x;
	dest->y = src.y;
}

Vector2i vec2i_add(Vector2i a, Vector2i b) {
	return vec2i(a.x + b.x, a.y + b.y);
}

Vector2i vec2i_sub(Vector2i a, Vector2i b) {
	return vec2i(a.x - b.x, a.y - b.y);
}

Vector2i vec2i_mul(Vector2i a, Vector2i b) {
	return vec2i(a.x * b.x, a.y * b.y);
}

Vector2i vec2i_div(Vector2i a, Vector2i b) {
	return vec2i(a.x / b.x, a.y / b.y);
}

Vector2i vec2i_negate(Vector2i a) {
	return vec2i(-a.x, -a.y);
}

uint64 vec2i_hash(Vector2i a) {
	// 2 32 bit values can fit directly into a 64 bit value.
	union {
		Vector2i v;
		uint64 hash;
	} shim;
	shim.v = a;
	return shim.hash;
}

bool vec2u_equals(Vector2u a, Vector2u b) {
	return a.x == b.x && a.y == b.y;
}

void vec2u_copy(Vector2u *dest, Vector2u src) {
	dest->x = src.x;
	dest->y = src.y;
}

Vector2u vec2u_add(Vector2u a, Vector2u b) {
	return vec2u(a.x + b.x, a.y + b.y);
}

Vector2u vec2u_sub(Vector2u a, Vector2u b) {
	return vec2u(a.x - b.x, a.y - b.y);
}

Vector2u vec2u_mul(Vector2u a, Vector2u b) {
	return vec2u(a.x * b.x, a.y * b.y);
}

Vector2u vec2u_div(Vector2u a, Vector2u b) {
	return vec2u(a.x / b.x, a.y / b.y);
}

Vector2u vec2u_negate(Vector2u a) {
	return vec2u(-a.x, -a.y);
}

bool vec3_equals(Vector3 a, Vector3 b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

void vec3_copy(Vector3 *dest, Vector3 src) {
	dest->x = src.x;
	dest->y = src.y;
	dest->z = src.z;
}

Vector3 vec3_add(Vector3 a, Vector3 b) {
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 vec3_sub(Vector3 a, Vector3 b) {
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 vec3_mul(Vector3 a, Vector3 b) {
	return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 vec3_mul_scalar(Vector3 a, float32 scalar) {
	return vec3(a.x * scalar, a.y * scalar, a.z * scalar);
}

Vector3 vec3_div(Vector3 a, Vector3 b) {
	return vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3 vec3_negate(Vector3 a) {
	return vec3(-a.x, -a.y, -a.z);
}

Vector3 vec3_normalize(Vector3 v) {
	float32 length = math_sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length > 0.0f) {
		float32 invLength = 1.0f / length;
		return vec3(v.x * invLength, v.y * invLength, v.z * invLength);
	}
	return v;
}

float32 vec3_dot(Vector3 a, Vector3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 vec3_cross(Vector3 a, Vector3 b) {
	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

// djb2 hash
uint64 vec3_hash(Vector3 a) {
	uint64 hash = 0;
	hash = (hash << 5) - hash + (uint64)a.x;
	hash = (hash << 5) - hash + (uint64)a.y;
	hash = (hash << 5) - hash + (uint64)a.z;
	return hash;
}

bool vec3i_equals(Vector3i a, Vector3i b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

void vec3i_copy(Vector3i *dest, Vector3i src) {
	dest->x = src.x;
	dest->y = src.y;
	dest->z = src.z;
}

Vector3i vec3i_add(Vector3i a, Vector3i b) {
	return vec3i(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3i vec3i_sub(Vector3i a, Vector3i b) {
	return vec3i(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3i vec3i_mul(Vector3i a, Vector3i b) {
	return vec3i(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3i vec3i_div(Vector3i a, Vector3i b) {
	return vec3i(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3i vec3i_negate(Vector3i a) {
	return vec3i(-a.x, -a.y, -a.z);
}

uint64 vec3i_hash(Vector3i a) {
	uint64 hash = 0;
	hash = (hash << 5) - hash + (uint64)a.x;
	hash = (hash << 5) - hash + (uint64)a.y;
	hash = (hash << 5) - hash + (uint64)a.z;
	return hash;
}

bool vec3u_equals(Vector3u a, Vector3u b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

void vec3u_copy(Vector3u *dest, Vector3u src) {
	dest->x = src.x;
	dest->y = src.y;
	dest->z = src.z;
}

Vector3u vec3u_add(Vector3u a, Vector3u b) {
	return vec3u(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3u vec3u_sub(Vector3u a, Vector3u b) {
	return vec3u(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3u vec3u_mul(Vector3u a, Vector3u b) {
	return vec3u(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3u vec3u_div(Vector3u a, Vector3u b) {
	return vec3u(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3u vec3u_negate(Vector3u a) {
	return vec3u(-a.x, -a.y, -a.z);
}