#ifndef FONT_H
#define FONT_H

#include "renderer/renderer.h"

typedef struct Font Font;

// Creates a font from a font file.
LS_EXPORT Font *renderer_create_font(const Renderer *renderer, String font_path, uint32 size);
// Destroys a font.
LS_EXPORT void font_destroy(Font *font);

// Draws text to the screen.
LS_EXPORT void font_draw(const Font *font, String text, Vector2 position, Vector2 scale, float32 rotation);

// Sets the size of the font.
LS_EXPORT void font_set_size(Font *font, uint32 size);
// Gets the size of the font.
LS_EXPORT uint32 font_get_size(const Font *font);

// Sets the color of the font.
LS_EXPORT void font_set_color(Font *font, Color color);

#endif // FONT_H