#ifndef UI_THEME_H
#define UI_THEME_H

#include "renderer/texture.h"

#include "modules/font/font.h"

typedef enum {
	UI_TEXT_ALIGN_LEFT,
	UI_TEXT_ALIGN_CENTER,
	UI_TEXT_ALIGN_RIGHT,
} UITextAlignment;

typedef struct {
	Color background_color;
	Color border_color;

	uint32 radius;
	uint32 border_size;

	Color font_color;
	uint32 font_size;

	const Font *font;
	const Texture *texture;

	UITextAlignment text_alignment;
} UIElementTheme;

#endif // UI_THEME_H