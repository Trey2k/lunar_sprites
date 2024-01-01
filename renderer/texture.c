#include "renderer/texture.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/texture.h"
#endif

struct TextureManager {
	Hashtable *parsers;
};

static struct TextureManager texture_manager;

void texture_manager_init() {
	texture_manager.parsers = hashtable_create(HASHTABLE_KEY_STRING, 16, false);
}

void texture_manager_deinit() {
	hashtable_destroy(texture_manager.parsers);
}

void texture_manager_register_parser(String extension, TextureParseFunc parse_func) {
	hashtable_set(texture_manager.parsers, HASH_KEY(str, extension), HASH_VAL(ptr, parse_func));
}

struct Texture {
	uint32 id;
	uint32 width;
	uint32 height;
};

Texture *texture_create(String path) {
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

	Texture *texture = texture_create_from_data(width, height, data);
	ls_free(data);

	return texture;
}

Texture *texture_create_from_data(uint32 width, uint32 height, const uint8 *data) {
#if defined(OPENGL_ENABLED)
	uint32 id = opengl_create_texture(width, height, data);
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

uint32 texture_get_width(const Texture *texture) {
	return texture->width;
}

uint32 texture_get_height(const Texture *texture) {
	return texture->height;
}