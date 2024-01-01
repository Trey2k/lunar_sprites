#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/core.h"

void texture_manager_init();
void texture_manager_deinit();

typedef void (*TextureParseFunc)(String path, uint32 *width, uint32 *height, uint8 **data);

LS_EXPORT void texture_manager_register_parser(String extension, TextureParseFunc parse_func);

typedef struct Texture Texture;

LS_EXPORT Texture *texture_create(String path);
LS_EXPORT Texture *texture_create_from_data(uint32 width, uint32 height, const uint8 *data);
LS_EXPORT void texture_destroy(Texture *texture);

LS_EXPORT void texture_bind(const Texture *texture, uint32 slot);
LS_EXPORT void texture_unbind(const Texture *texture);

LS_EXPORT uint32 texture_get_width(const Texture *texture);
LS_EXPORT uint32 texture_get_height(const Texture *texture);

#endif // TEXTURE_H