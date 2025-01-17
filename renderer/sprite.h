#ifndef SPRITE_H
#define SPRITE_H

#include "renderer/renderer.h"
#include "renderer/texture.h"

typedef struct Sprite Sprite;

#define SPRITE_OBJECT_TYPE "Sprite"
void register_sprite_object();

uint32 sprite_get_object_type();

// Creates a empty sprite.
LS_EXPORT Sprite *sprite_create_empty();
// Creates a sprite from an image file.
LS_EXPORT Sprite *sprite_create(String image_path, Vector2i position, Vector2 scale, float32 rotation);
// Destroys a sprite.
LS_EXPORT void sprite_destroy(Sprite *sprite);

// Draws a sprite to the screen.
LS_EXPORT void sprite_draw(Sprite *sprite);

// Sets the position of the sprite.
LS_EXPORT void sprite_set_position(Sprite *sprite, Vector2i position);
// Gets the position of the sprite.
LS_EXPORT Vector2i sprite_get_position(const Sprite *sprite);

// Sets the scale of the sprite.
LS_EXPORT void sprite_scale(Sprite *sprite, Vector2 scale);

// Sets the rotation of the sprite.
LS_EXPORT void sprite_set_rotation(Sprite *sprite, float32 rotation);
// Gets the rotation of the sprite.
LS_EXPORT float32 sprite_get_rotation(const Sprite *sprite);

#endif // SPRITE_H