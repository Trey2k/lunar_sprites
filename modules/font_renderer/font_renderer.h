#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include "core/core.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"

typedef struct Font Font;

void font_renderer_init(Renderer *renderer, LSCore *core);
void font_renderer_deinit();

LS_EXPORT Font *font_create(String font_path);
LS_EXPORT void font_destroy(Font *font);

LS_EXPORT void font_renderer_set_color(Color color);
LS_EXPORT void font_renderer_draw_text(const Font *font, String text, float32 x, float32 y, uint32 size);

Texture *font_renderer_create_atlas(uint32 atlas_width, uint32 atlas_height);
void font_renderer_bitmap_to_atlas(Texture *atlas, uint8 *bitmap, float32 width, float32 height, float32 x, float32 y);
void font_renderer_render_text(Texture *atlas, float32 *vertices, float32 *tcoords, size_t nverts);
void font_renderer_free_atlas(Texture *atlas);

#endif // FONT_RENDERER_H