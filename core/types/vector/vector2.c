#include "core/types/vector/vector2.h"

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