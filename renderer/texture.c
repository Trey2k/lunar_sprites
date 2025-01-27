#include "renderer/texture.h"
#include "core/resource/resource.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/texture.h"
#endif

#include "core/resource/resource_db.h"

#include <stdarg.h>

struct TextureManager {
	Hashtable *parsers;

	uint32 resource_type;
};

static struct TextureManager texture_manager;

static void *texture_resource_create(String path);
static void texture_resource_destroy(void *texture);

void texture_manager_init() {
	texture_manager.parsers = hashtable_create(HASHTABLE_KEY_STRING, 16, false);

	texture_manager.resource_type = resource_db_register_type(TEXTURE_RESOURCE_TYPE, ".png,.jpg,.jpeg,.jpg,.bmp",
			texture_resource_create, texture_resource_destroy);
}

void texture_manager_deinit() {
	hashtable_destroy(texture_manager.parsers);
}

void texture_manager_register_parser(String extension, TextureParseFunc parse_func) {
	hashtable_set(texture_manager.parsers, HASH_KEY(str, extension), HASH_VAL(ptr, parse_func));
}

uint32 texture_manager_get_resource_type() {
	return texture_manager.resource_type;
}

struct Texture {
	uint32 id;
	uint32 width;
	uint32 height;
	bool is_atlas;
};

Texture *texture_create_from_image(String path) {
	String extension = os_path_get_extension(path);

	TextureParseFunc parse_func = hashtable_get(texture_manager.parsers, HASH_KEY(str, extension)).ptr;

	if (parse_func == NULL) {
		ls_log(LOG_LEVEL_ERROR, "No texture parser found for extension %s\n", extension);
		return NULL;
	}

	uint8 *data = NULL;
	uint32 width = 0;
	uint32 height = 0;

	parse_func(path, &width, &height, &data);
	if (data == NULL) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load texture %s\n", path);
		return NULL;
	}
	Texture *texture = texture_create(width, height, TEXTURE_FORMAT_RGBA, data);
	ls_free(data);

	return texture;
}

Texture *texture_create(uint32 width, uint32 height, TextureFormat format, const uint8 *data) {
#if defined(OPENGL_ENABLED)
	uint32 id = opengl_create_texture(width, height, format, data);
#else
	uint32 id = 0;
#endif

	Texture *texture = ls_malloc(sizeof(Texture));
	texture->id = id;
	texture->width = width;
	texture->height = height;

	return texture;
}

void texture_destroy(Texture *texture) {
#if defined(OPENGL_ENABLED)
	opengl_destroy_texture(texture->id);
#endif
	ls_free(texture);
}

void texture_bind(const Texture *texture, uint32 slot) {
#if defined(OPENGL_ENABLED)
	opengl_bind_texture(texture->id, slot);
#endif
}

void texture_unbind(const Texture *texture) {
#if defined(OPENGL_ENABLED)
	opengl_bind_texture(texture->id, 0);
#endif
}

void texture_add_sub_texture(Texture *texture, TextureFormat format, const uint8 *data, float32 x, float32 y, float32 width, float32 height) {
#if defined(OPENGL_ENABLED)
	opengl_texture_add_sub_texture(texture->id, format, data, x, y, width, height);
#endif
}

uint32 texture_get_width(const Texture *texture) {
	return texture->width;
}

uint32 texture_get_height(const Texture *texture) {
	return texture->height;
}

Resource *texture_to_resource(const Texture *texture) {
	return resource_create_from_data(TEXTURE_RESOURCE_TYPE, (void *)texture);
}

Texture *texture_from_resource(Resource *resource) {
	if (texture_manager.resource_type != resource_get_type_id(resource)) {
		ls_log(LOG_LEVEL_ERROR, "Resource is not a texture\n");
		return NULL;
	}
	return (Texture *)resource_get_data(resource);
}

static void *texture_resource_create(String path) {
	return (void *)texture_create_from_image(path);
}

static void texture_resource_destroy(void *texture) {
	texture_destroy((Texture *)texture);
}