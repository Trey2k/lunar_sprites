#include "renderer/opengl/buffers.h"
#include "renderer/buffers.h"
#include "renderer/opengl/debug.h"

#include <glad/gl.h>

struct OpenGLVertexBuffer {
	uint32 id;
	uint32 count;
	const BufferLayout *buffer_layout;
	BufferUsage usage;
};

static GLenum buffer_usage_to_gl(BufferUsage usage) {
	switch (usage) {
		case BUFFER_USAGE_STATIC:
			return GL_STATIC_DRAW;
		case BUFFER_USAGE_DYNAMIC:
			return GL_DYNAMIC_DRAW;
		case BUFFER_USAGE_STREAM:
			return GL_STREAM_DRAW;
		default:
			LS_ASSERT_MSG(false, "Unknown buffer usage: %d", usage);
			return 0;
	}
}

OpenGLVertexBuffer *opengl_create_vertex_buffer_empty(BufferUsage usage) {
	OpenGLVertexBuffer *vertex_buffer = ls_malloc(sizeof(OpenGLVertexBuffer));
	vertex_buffer->usage = usage;
	GL_CALL(glGenBuffers(1, &vertex_buffer->id));
	return vertex_buffer;
}

OpenGLVertexBuffer *opengl_create_vertex_buffer(const void *data, uint32 size, BufferUsage usage) {
	OpenGLVertexBuffer *vertex_buffer = ls_malloc(sizeof(OpenGLVertexBuffer));
	vertex_buffer->usage = usage;
	GL_CALL(glGenBuffers(1, &vertex_buffer->id));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, buffer_usage_to_gl(usage)));
	return vertex_buffer;
}

void opengl_vertex_buffer_destroy(OpenGLVertexBuffer *vertex_buffer) {
	GL_CALL(glDeleteBuffers(1, &vertex_buffer->id));
	ls_free(vertex_buffer);
}

void opengl_vertex_buffer_set_data(OpenGLVertexBuffer *vertex_buffer, const void *data, uint32 size) {
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, buffer_usage_to_gl(vertex_buffer->usage)));
	vertex_buffer->count = size / buffer_layout_get_stride(vertex_buffer->buffer_layout);
}

void opengl_vertex_buffer_set_sub_data(OpenGLVertexBuffer *vertex_buffer, const void *data, uint32 size, uint32 offset) {
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id));
	GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
	vertex_buffer->count = size / buffer_layout_get_stride(vertex_buffer->buffer_layout);
}

void opengl_vertex_buffer_bind(const OpenGLVertexBuffer *vertex_buffer) {
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id));
}

void opengl_vertex_buffer_unbind(const OpenGLVertexBuffer *vertex_buffer) {
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void opengl_vertex_buffer_set_layout(OpenGLVertexBuffer *vertex_buffer, const BufferLayout *buffer_layout) {
	vertex_buffer->buffer_layout = buffer_layout;
}

const BufferLayout *opengl_vertex_buffer_get_layout(const OpenGLVertexBuffer *vertex_buffer) {
	return vertex_buffer->buffer_layout;
}

uint32 opengl_vertex_buffer_get_count(const OpenGLVertexBuffer *vertex_buffer) {
	return vertex_buffer->count;
}

struct OpenGLIndexBuffer {
	uint32 id;
	uint32 count;

	BufferUsage usage;
};

OpenGLIndexBuffer *opengl_index_buffer_create_empty(BufferUsage usage) {
	OpenGLIndexBuffer *index_buffer = ls_malloc(sizeof(OpenGLIndexBuffer));
	index_buffer->usage = usage;
	GL_CALL(glGenBuffers(1, &index_buffer->id));
	return index_buffer;
}

OpenGLIndexBuffer *opengl_index_buffer_create(const void *data, uint32 size, BufferUsage usage) {
	OpenGLIndexBuffer *index_buffer = ls_malloc(sizeof(OpenGLIndexBuffer));
	index_buffer->usage = usage;
	GL_CALL(glGenBuffers(1, &index_buffer->id));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, buffer_usage_to_gl(usage)));

	index_buffer->count = size / sizeof(uint32);
	return index_buffer;
}

void opengl_index_buffer_destroy(OpenGLIndexBuffer *index_buffer) {
	GL_CALL(glDeleteBuffers(1, &index_buffer->id));
	ls_free(index_buffer);
}

void opengl_index_buffer_bind(const OpenGLIndexBuffer *index_buffer) {
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id));
}

void opengl_index_buffer_unbind(const OpenGLIndexBuffer *index_buffer) {
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void opengl_index_buffer_set_data(const OpenGLIndexBuffer *index_buffer, const void *data, uint32 size) {
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, buffer_usage_to_gl(index_buffer->usage)));
}

void opengl_index_buffer_set_sub_data(const OpenGLIndexBuffer *index_buffer, const void *data, uint32 size, uint32 offset) {
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id));
	GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data));
}

uint32 opengl_index_buffer_get_count(const OpenGLIndexBuffer *index_buffer) {
	return index_buffer->count;
}