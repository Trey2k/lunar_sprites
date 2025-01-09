#ifndef FONT_H
#define FONT_H

#include "core/core.h"
#include "core/types/typedefs.h"

#include "renderer/renderer.h"
#include "renderer/texture.h"

typedef struct Font Font;

void font_renderer_init(Renderer *renderer, LSCore *core, const LSWindow *window);
void font_renderer_deinit();

typedef void (*FontRenderTextCallback)(const Texture *atlas, float32 *verts, float32 *tcoords, size_t nverts, void *user_data);

LS_EXPORT Font *font_create(String font_path);
LS_EXPORT void font_destroy(Font *font);

LS_EXPORT Vector2u font_draw_text(const Font *font, uint32 font_size, Color font_color, String text, Vector2 position);
LS_EXPORT Vector2u font_get_text_size(const Font *font, uint32 font_size, String text);

#endif // FONT_H