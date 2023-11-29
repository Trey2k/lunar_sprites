#include "core/math/vector.h"

bool vec2_equals(Vector2 a, Vector2 b) {
	return a.x == b.x && a.y == b.y;
}

void vec2_copy(Vector2 *dest, Vector2 src) {
	dest->x = src.x;
	dest->y = src.y;
}

bool vec2i_equals(Vector2i a, Vector2i b) {
	return a.x == b.x && a.y == b.y;
}

void vec2i_copy(Vector2i *dest, Vector2i src) {
	dest->x = src.x;
	dest->y = src.y;
}

bool vec3_equals(Vector3 a, Vector3 b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

void vec3_copy(Vector3 *dest, Vector3 src) {
	dest->x = src.x;
	dest->y = src.y;
	dest->z = src.z;
}

bool vec3i_equals(Vector3i a, Vector3i b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

void vec3i_copy(Vector3i *dest, Vector3i src) {
	dest->x = src.x;
	dest->y = src.y;
	dest->z = src.z;
}