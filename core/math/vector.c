#include "core/math/vector.h"

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

Vector3 vec3_div(Vector3 a, Vector3 b) {
	return vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vector3 vec3_negate(Vector3 a) {
	return vec3(-a.x, -a.y, -a.z);
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