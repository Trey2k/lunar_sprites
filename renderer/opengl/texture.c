#include "renderer/opengl/texture.h"
#include "renderer/opengl/debug.h"

#include <glad/gl.h>

static void flip_data(uint32 width, uint32 height, uint8 *data) {
	uint32 row_size = width * 4;
	uint8 *row = ls_malloc(row_size);

	for (uint32 i = 0; i < height / 2; i++) {
		uint8 *top = data + i * row_size;
		uint8 *bottom = data + (height - i - 1) * row_size;

		ls_memcpy(row, top, row_size);
		ls_memcpy(top, bottom, row_size);
		ls_memcpy(bottom, row, row_size);
	}

	ls_free(row);
}

uint32 opengl_create_texture(uint32 width, uint32 height, const uint8 *data) {
	uint32 texture;

	uint8 *opengl_data = ls_malloc(width * height * 4);
	ls_memcpy(opengl_data, data, width * height * 4);
	flip_data(width, height, opengl_data);

	GL_CALL(glGenTextures(1, &texture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_data));

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	ls_free(opengl_data);
	return texture;
}

void opengl_destroy_texture(uint32 texture) {
	GL_CALL(glDeleteTextures(1, &texture));
}

void opengl_bind_texture(uint32 texture, uint32 slot) {
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
}