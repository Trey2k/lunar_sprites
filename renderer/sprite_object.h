#ifndef SPRITE_OBJECT_H
#define SPRITE_OBJECT_H

#include "core/core.h"
#include "core/types/bstring.h"
#include "renderer/sprite.h"

// TODO: Make api export this
static const BString SPRITE_OBJECT_TYPE = BSTRING_CONST_LENGTH("Sprite", 6);
void register_sprite_object();

LS_EXPORT uint64 sprite_get_object_type();

LS_EXPORT bool object_is_sprite(Object *object);
LS_EXPORT Sprite *object_to_sprite(Object *object);
LS_EXPORT Object *sprite_to_object(Sprite *sprite);

#endif // SPRITE_OBJECT_H