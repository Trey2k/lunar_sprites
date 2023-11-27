#include "renderer/opengl/renderer_interface.h"
#include "renderer/opengl/debug.h"
#include "renderer/renderer.h"
#include <glad/gl.h>

void opengl_set_clear_color(float32 r, float32 g, float32 b, float32 a) {
	GL_CALL(glClearColor(r, g, b, a));
}

void opengl_clear() {
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

uint32 opengl_create_vertex_buffer(size_t size, void *data, UsageHint usage_hint) {
	GLenum gl_usage_hint = 0;

	switch (usage_hint) {
		case USAGE_HINT_STREAM_DRAW:
			gl_usage_hint = GL_STREAM_DRAW;
			break;
		case USAGE_HINT_STREAM_READ:
			gl_usage_hint = GL_STREAM_READ;
			break;
		case USAGE_HINT_STREAM_COPY:
			gl_usage_hint = GL_STREAM_COPY;
			break;
		case USAGE_HINT_STATIC_DRAW:
			gl_usage_hint = GL_STATIC_DRAW;
			break;
		case USAGE_HINT_STATIC_READ:
			gl_usage_hint = GL_STATIC_READ;
			break;
		case USAGE_HINT_STATIC_COPY:
			gl_usage_hint = GL_STATIC_COPY;
			break;
		case USAGE_HINT_DYNAMIC_DRAW:
			gl_usage_hint = GL_DYNAMIC_DRAW;
			break;
		case USAGE_HINT_DYNAMIC_READ:
			gl_usage_hint = GL_DYNAMIC_READ;
			break;
		case USAGE_HINT_DYNAMIC_COPY:
			gl_usage_hint = GL_DYNAMIC_COPY;
			break;
		default:
			ls_log(LOG_LEVEL_ERROR, "Invalid usage hint: %d", usage_hint);
			return 0;
	}

	GL_CALL(uint32 vertex_buffer);
	GL_CALL(glGenBuffers(1, &vertex_buffer));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, gl_usage_hint));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	return vertex_buffer;
}

void opengl_destroy_vertex_buffer(uint32 vertex_buffer) {
	GL_CALL(glDeleteBuffers(1, &vertex_buffer));
}

void opengl_bind_vertex_buffer(TargetHint target, uint32 vertex_buffer) {
	GLenum gl_target = 0;

	switch (target) {
		case TARGET_HINT_ARRAY_BUFFER:
			gl_target = GL_ARRAY_BUFFER;
			break;
		case TARGET_HINT_ELEMENT_ARRAY_BUFFER:
			gl_target = GL_ELEMENT_ARRAY_BUFFER;
			break;
		case TARGET_HINT_PIXEL_PACK_BUFFER:
			gl_target = GL_PIXEL_PACK_BUFFER;
			break;
		case TARGET_HINT_PIXEL_UNPACK_BUFFER:
			gl_target = GL_PIXEL_UNPACK_BUFFER;
			break;
		case TARGET_HINT_COPY_READ_BUFFER:
			gl_target = GL_COPY_READ_BUFFER;
			break;
		case TARGET_HINT_COPY_WRITE_BUFFER:
			gl_target = GL_COPY_WRITE_BUFFER;
			break;
		default:
			ls_log(LOG_LEVEL_ERROR, "Invalid target hint: %d", target);
			return;
	}

	GL_CALL(glBindBuffer(gl_target, vertex_buffer));
}

uint32 opengl_create_vertex_array() {
	GL_CALL(uint32 vertex_array);
	GL_CALL(glGenVertexArrays(1, &vertex_array));
	return vertex_array;
}

void opengl_destroy_vertex_array(uint32 vertex_array) {
	GL_CALL(glDeleteVertexArrays(1, &vertex_array));
}

void opengl_bind_vertex_array(uint32 vertex_array) {
	GL_CALL(glBindVertexArray(vertex_array));
}

void opengl_set_vertex_array_uniform(uint32 vertex_array, int32 uniform_position, uint32 size, DataType data_type, bool normalized, size_t stride, size_t offset) {
	GLenum gl_data_type = 0;

	LS_ASSERT(uniform_position < GL_MAX_VERTEX_ATTRIBS);
	LS_ASSERT(size >= 1 && size <= 4);
	LS_ASSERT(stride >= 0);

	switch (data_type) {
		case DATA_TYPE_BYTE:
			gl_data_type = GL_BYTE;
			break;
		case DATA_TYPE_UNSIGNED_BYTE:
			gl_data_type = GL_UNSIGNED_BYTE;
			break;
		case DATA_TYPE_SHORT:
			gl_data_type = GL_SHORT;
			break;
		case DATA_TYPE_UNSIGNED_SHORT:
			gl_data_type = GL_UNSIGNED_SHORT;
			break;
		case DATA_TYPE_INT:
			gl_data_type = GL_INT;
			break;
		case DATA_TYPE_UNSIGNED_INT:
			gl_data_type = GL_UNSIGNED_INT;
			break;
		case DATA_TYPE_FLOAT:
			gl_data_type = GL_FLOAT;
			break;
		case DATA_TYPE_DOUBLE:
			gl_data_type = GL_DOUBLE;
			break;
		default:
			ls_log(LOG_LEVEL_ERROR, "Invalid data type: %d", data_type);
			return;
	}

	GL_CALL(glVertexAttribPointer(uniform_position, size, gl_data_type, normalized, stride, (void *)offset));
}

void opengl_enable_vertex_attrib_array(uint32 index) {
	GL_CALL(glEnableVertexAttribArray(index));
}

void opengl_draw_arrays(DrawMode draw_mode, size_t first, size_t count) {
	GLenum gl_draw_mode = 0;

	switch (draw_mode) {
		case DRAW_MODE_POINTS:
			gl_draw_mode = GL_POINTS;
			break;
		case DRAW_MODE_LINE_STRIP:
			gl_draw_mode = GL_LINE_STRIP;
			break;
		case DRAW_MODE_LINE_LOOP:
			gl_draw_mode = GL_LINE_LOOP;
			break;
		case DRAW_MODE_LINES:
			gl_draw_mode = GL_LINES;
			break;
		case DRAW_MODE_TRIANGLE_STRIP:
			gl_draw_mode = GL_TRIANGLE_STRIP;
			break;
		case DRAW_MODE_TRIANGLE_FAN:
			gl_draw_mode = GL_TRIANGLE_FAN;
			break;
		case DRAW_MODE_TRIANGLES:
			gl_draw_mode = GL_TRIANGLES;
			break;
		default:
			ls_log(LOG_LEVEL_ERROR, "Invalid draw mode: %d", draw_mode);
			return;
	}

	GL_CALL(glDrawArrays(gl_draw_mode, first, count));
}