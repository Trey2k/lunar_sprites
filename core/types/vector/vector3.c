#include "core/types/vector/vector3.h"

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