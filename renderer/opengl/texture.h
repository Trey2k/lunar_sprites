#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H

#include "core/core.h"

uint32 opengl_create_texture(uint32 width, uint32 height, const uint8 *data);
void opengl_destroy_texture(uint32 texture);

void opengl_bind_texture(uint32 texture, uint32 slot);

#endif // OPENGL_TEXTURE_H
