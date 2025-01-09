#ifndef SPRITE_H
#define SPRITE_H

#include "renderer/renderer.h"
#include "renderer/texture.h"

typedef struct Sprite Sprite;

// Creates a sprite from an image file.
LS_EXPORT Sprite *renderer_create_sprite(const Renderer *renderer, String image_path, Vector2 position, Vector2 scale, float32 rotation);
// Creates a sprite from a Texture. The sprite will take ownership of the texture.
LS_EXPORT Sprite *renderer_create_sprite_texture(const Renderer *renderer, Texture *texture, Vector2 position, Vector2 scale, float32 rotation);
// Destroys a sprite.
LS_EXPORT void sprite_destroy(Sprite *sprite);

// Draws a sprite to the screen.
LS_EXPORT void sprite_draw(Sprite *sprite);

// Sets the position of the sprite.
LS_EXPORT void sprite_set_position(Sprite *sprite, Vector2 position);
// Gets the position of the sprite.
LS_EXPORT Vector2 sprite_get_position(const Sprite *sprite);

// Sets the scale of the sprite.
LS_EXPORT void sprite_set_scale(Sprite *sprite, Vector2 scale);
// Gets the scale of the sprite.
LS_EXPORT Vector2 sprite_get_scale(const Sprite *sprite);

// Sets the rotation of the sprite.
LS_EXPORT void sprite_set_rotation(Sprite *sprite, float32 rotation);
// Gets the rotation of the sprite.
LS_EXPORT float32 sprite_get_rotation(const Sprite *sprite);

#endif // SPRITE_H