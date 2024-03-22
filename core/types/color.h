#ifndef COLOR_H
#define COLOR_H

#include "core/types/typedefs.h"

typedef struct Color {
	float32 r;
	float32 g;
	float32 b;
	float32 a;
} Color;

#define COLOR_WHITE \
	(Color) { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f }

#define COLOR_BLACK \
	(Color) { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f }

#define COLOR_RED \
	(Color) { .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f }

#define COLOR_GREEN \
	(Color) { .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f }

#define COLOR_BLUE \
	(Color) { .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f }

#define COLOR_YELLOW \
	(Color) { .r = 1.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f }

#define COLOR_CYAN \
	(Color) { .r = 0.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f }

#define COLOR_MAGENTA \
	(Color) { .r = 1.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f }

#define COLOR_TRANSPARENT \
	(Color) { .r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f }

#define COLOR_GREY \
	(Color) { .r = 0.5f, .g = 0.5f, .b = 0.5f, .a = 1.0f }

#define COLOR_PINK \
	(Color) { .r = 1.0f, .g = 0.0f, .b = 0.5f, .a = 1.0f }

#define COLOR_ORANGE \
	(Color) { .r = 1.0f, .g = 0.5f, .b = 0.0f, .a = 1.0f }

#define COLOR_PURPLE \
	(Color) { .r = 0.5f, .g = 0.0f, .b = 1.0f, .a = 1.0f }

#define COLOR_BROWN \
	(Color) { .r = 0.5f, .g = 0.25f, .b = 0.0f, .a = 1.0f }

#define COLOR_LIME \
	(Color) { .r = 0.5f, .g = 1.0f, .b = 0.0f, .a = 1.0f }

#define COLOR_TEAL \
	(Color) { .r = 0.0f, .g = 0.5f, .b = 0.5f, .a = 1.0f }

#define COLOR_AQUA \
	(Color) { .r = 0.0f, .g = 1.0f, .b = 0.5f, .a = 1.0f }

#define COLOR_MAROON \
	(Color) { .r = 0.5f, .g = 0.0f, .b = 0.0f, .a = 1.0f }

#define COLOR_NAVY \
	(Color) { .r = 0.0f, .g = 0.0f, .b = 0.5f, .a = 1.0f }

#define COLOR_OLIVE \
	(Color) { .r = 0.5f, .g = 0.5f, .b = 0.0f, .a = 1.0f }

#define COLOR_SILVER \
	(Color) { .r = 0.75f, .g = 0.75f, .b = 0.75f, .a = 1.0f }

#define COLOR_GOLD \
	(Color) { .r = 1.0f, .g = 0.84f, .b = 0.0f, .a = 1.0f }

#define COLOR_BRONZE \
	(Color) { .r = 0.8f, .g = 0.5f, .b = 0.2f, .a = 1.0f }

#define COLOR_TURQUOISE \
	(Color) { .r = 0.25f, .g = 0.88f, .b = 0.82f, .a = 1.0f }

// Creates a color from RGBA values.
_FORCE_INLINE_ Color color_create(float32 r, float32 g, float32 b, float32 a) {
	return (Color){ .r = r, .g = g, .b = b, .a = a };
}

// Creates a color from RGB values.
_FORCE_INLINE_ Color color_create_rgb(float32 r, float32 g, float32 b) {
	return color_create(r, g, b, 1.0f);
}

// Creates a color from a hex value.
_FORCE_INLINE_ Color color_create_hex(uint32 hex) {
	return color_create(
			((hex >> 16) & 0xFF) / 255.0f,
			((hex >> 8) & 0xFF) / 255.0f,
			(hex & 0xFF) / 255.0f,
			1.0f);
}

// Creates a color from a hex value and alpha value.
_FORCE_INLINE_ Color color_create_hex_a(uint32 hex, float32 a) {
	return color_create(
			((hex >> 16) & 0xFF) / 255.0f,
			((hex >> 8) & 0xFF) / 255.0f,
			(hex & 0xFF) / 255.0f,
			a);
}

// Combines 2 color values
_FORCE_INLINE_ Color color_add(Color a, Color b) {
	return color_create(
			(a.r + b.r) / 2.0f,
			(a.g + b.g) / 2.0f,
			(a.b + b.b) / 2.0f,
			(a.a + b.a) / 2.0f);
}

// Subtracts 2 color values
_FORCE_INLINE_ Color color_subtract(Color a, Color b) {
	return color_create(
			(a.r - b.r) / 2.0f,
			(a.g - b.g) / 2.0f,
			(a.b - b.b) / 2.0f,
			(a.a - b.a) / 2.0f);
}

// Multiplies 2 color values
_FORCE_INLINE_ Color color_multiply(Color a, Color b) {
	return color_create(
			a.r * b.r,
			a.g * b.g,
			a.b * b.b,
			a.a * b.a);
}

// Divides 2 color values
_FORCE_INLINE_ Color color_divide(Color a, Color b) {
	return color_create(
			a.r / b.r,
			a.g / b.g,
			a.b / b.b,
			a.a / b.a);
}

// Compares 2 color values
_FORCE_INLINE_ bool color_equals(Color a, Color b) {
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

#endif // COLOR_H