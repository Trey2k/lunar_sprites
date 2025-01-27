#ifndef SPRITE_H
#define SPRITE_H

#include "renderer/renderer.h"
#include "renderer/texture.h"

typedef struct Sprite Sprite;

// Creates a empty sprite.
LS_EXPORT Sprite *sprite_create_empty();
// Creates a sprite from an image file.
LS_EXPORT Sprite *sprite_create(BString image_path, Vector2i position, Vector2 scale, float32 rotation);
// Destroys a sprite.
LS_EXPORT void sprite_destroy(Sprite *sprite);

// Draws a sprite to the screen.
LS_EXPORT void sprite_draw(Sprite *sprite);

// Sets the position of the sprite.
LS_EXPORT void sprite_set_position(Sprite *sprite, Vector2i position);
// Gets the position of the sprite.
LS_EXPORT Vector2i sprite_get_position(const Sprite *sprite);

LS_EXPORT void sprite_set_texture(Sprite *sprite, Resource *texture);
LS_EXPORT Resource *sprite_get_texture(const Sprite *sprite);

LS_EXPORT Vector2u sprite_get_size(const Sprite *sprite);

// Sets the scale of the sprite.
LS_EXPORT void sprite_set_scale(Sprite *sprite, Vector2 scale);
// Gets the scale of the sprite.
LS_EXPORT Vector2 sprite_get_scale(const Sprite *sprite);

// Sets the rotation of the sprite.
LS_EXPORT void sprite_set_rotation(Sprite *sprite, float32 rotation);
// Gets the rotation of the sprite.
LS_EXPORT float32 sprite_get_rotation(const Sprite *sprite);

LS_EXPORT BString sprite_to_string(const Sprite *sprite);

#endif // SPRITE_H