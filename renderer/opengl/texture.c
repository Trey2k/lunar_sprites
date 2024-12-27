#include "renderer/opengl/texture.h"
#include "renderer/opengl/debug.h"
#include "renderer/opengl/utils.h"

#include <glad/gl.h>

static inline GLenum texture_format_to_gl(TextureFormat format) {
	switch (format) {
		case TEXTURE_FORMAT_RGBA:
			return GL_RGBA;
		case TEXTURE_FORMAT_RGB:
			return GL_RGB;
		case TEXTURE_FORMAT_RG:
			return GL_RG;
		case TEXTURE_FORMAT_R:
			return GL_RED;
		default:
			LS_ASSERT_MSG(false, "Unknown texture format: %d", format);
			return 0;
	}
}

static inline GLenum texture_format_to_internal_gl(TextureFormat format) {
	switch (format) {
		case TEXTURE_FORMAT_RGBA:
			return GL_RGBA8;
		case TEXTURE_FORMAT_RGB:
			return GL_RGB8;
		case TEXTURE_FORMAT_RG:
			return GL_RG8;
		case TEXTURE_FORMAT_R:
			return GL_R8;
		default:
			LS_ASSERT_MSG(false, "Unknown texture format: %d", format);
			return 0;
	}
}

uint32 opengl_create_texture(uint32 width, uint32 height, TextureFormat format, const uint8 *data) {
	uint32 texture;

	opengl_bind_texture(0, 0);
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glGenTextures(1, &texture));
	opengl_bind_texture(texture, 0);

	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, texture_format_to_internal_gl(format), width, height, 0, texture_format_to_gl(format), GL_UNSIGNED_BYTE, data));

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	if (format == TEXTURE_FORMAT_R) {
		static GLint swizzleRgbaParams[4] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
		GL_CALL(glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleRgbaParams));
	}

	opengl_bind_texture(0, 0);

	return texture;
}

void opengl_destroy_texture(uint32 texture) {
	GL_CALL(glDeleteTextures(1, &texture));
}

void opengl_bind_texture(uint32 texture, uint32 slot) {
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
}

static inline void atlas_push_pixel_values(GLint alignment, GLint row_length, GLint skip_pixels, GLint skip_rows) {
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, alignment));
	GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, row_length));
	GL_CALL(glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixels));
	GL_CALL(glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows));
}

void opengl_texture_add_sub_texture(uint32 texture, TextureFormat format, const uint8 *data, float32 x, float32 y, float32 width, float32 height) {
	GLint alignment, row_length, skip_pixels, skip_rows;
	GL_CALL(glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment));
	GL_CALL(glGetIntegerv(GL_UNPACK_ROW_LENGTH, &row_length));
	GL_CALL(glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skip_pixels));
	GL_CALL(glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skip_rows));

	opengl_bind_texture(texture, 0);
	atlas_push_pixel_values(1, width, 0, 0);
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, texture_format_to_gl(format), GL_UNSIGNED_BYTE, data));
	atlas_push_pixel_values(alignment, row_length, skip_pixels, skip_rows);
}