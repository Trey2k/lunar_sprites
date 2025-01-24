#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "core/core.h"
#include "renderer/sprite.h"

#define SPRITE_OBJECT_TYPE "Sprite"
void register_sprite_object();

LS_EXPORT uint32 sprite_get_object_type();

LS_EXPORT bool object_is_sprite(Object *object);
LS_EXPORT Sprite *object_to_sprite(Object *object);
LS_EXPORT Object *sprite_to_object(Sprite *sprite);

#endif // SPRITE_OBJECT_H