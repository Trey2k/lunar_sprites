#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/core.h"

#include "renderer/renderer.h"

void texture_manager_init();
void texture_manager_deinit();

typedef void (*TextureParseFunc)(String path, uint32 *width, uint32 *height, uint8 **data);

LS_EXPORT void texture_manager_register_parser(String extension, TextureParseFunc parse_func);

typedef enum {
	TEXTURE_FORMAT_NONE = 0,
	TEXTURE_FORMAT_R,
	TEXTURE_FORMAT_A,
	TEXTURE_FORMAT_RG,
	TEXTURE_FORMAT_RGB,
	TEXTURE_FORMAT_RGBA,
} TextureFormat;

typedef struct Texture Texture;

LS_EXPORT Texture *texture_create(uint32 width, uint32 height, TextureFormat format, const uint8 *data);
LS_EXPORT Texture *texture_create_from_image(String path);
LS_EXPORT void texture_destroy(Texture *texture);

LS_EXPORT void texture_bind(const Texture *texture, uint32 slot);
LS_EXPORT void texture_unbind(const Texture *texture);

LS_EXPORT uint32 texture_get_width(const Texture *texture);
LS_EXPORT uint32 texture_get_height(const Texture *texture);

LS_EXPORT void texture_add_sub_texture(Texture *texture, TextureFormat format, const uint8 *data, float32 x, float32 y, float32 width, float32 height);

#endif // TEXTURE_H