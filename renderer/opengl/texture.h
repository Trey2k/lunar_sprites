#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "core/core.h"
#include "renderer/texture.h"

uint32 opengl_create_texture(uint32 width, uint32 height, TextureFormat format, const uint8 *data);
void opengl_destroy_texture(uint32 texture);

void opengl_bind_texture(uint32 texture, uint32 slot);
void opengl_texture_add_sub_texture(uint32 texture, TextureFormat format, const uint8 *data, float32 x, float32 y, float32 width, float32 height);

#endif // OPENGL_TEXTURE_H
