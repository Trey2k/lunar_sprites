#include "renderer/font.h"
#include "core/types/color.h"
#include "core/types/typedefs.h"

extern const char *const FONT_SHADER_SOURCE;

struct Font {
	const Renderer *renderer;
	uint32 size;
	Color color;
};

Font *renderer_create_font(const Renderer *renderer, String font_path, uint32 size) {
	Font *font = ls_malloc(sizeof(Font));
	font->renderer = renderer;
	font->size = size;
	font->color = COLOR_WHITE;

	return font;
}