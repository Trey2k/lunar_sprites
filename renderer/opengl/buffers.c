#include "renderer/opengl/buffers.h"
#include "renderer/buffers.h"
#include "renderer/opengl/debug.h"

#include <glad/gl.h>

struct OpenGLVertexBuffer {
	uint32 id;
	const BufferLayout *buffer_layout;
};

OpenGLVertexBuffer *opengl_create_vertex_buffer(const float32 *data, uint32 size) {
	OpenGLVertexBuffer *vertex_buffer = ls_malloc(sizeof(OpenGLVertexBuffer));
	GL_CALL(glGenBuffers(1, &vertex_buffer->id));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->id));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	return vertex_buffer;
}

void opengl_vertex_buffer_destroy(OpenGLVertexBuffer *vertex_buffer) {
	GL_CALL(glDeleteBuffers(1, &vertex_buffer->id));
	ls_free(vertex_buffer);
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

struct OpenGLIndexBuffer {
	uint32 id;
	uint32 count;
};

OpenGLIndexBuffer *opengl_index_buffer_create(const uint32 *data, uint32 size) {
	OpenGLIndexBuffer *index_buffer = ls_malloc(sizeof(OpenGLIndexBuffer));
	GL_CALL(glGenBuffers(1, &index_buffer->id));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->id));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

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

uint32 opengl_index_buffer_get_count(const OpenGLIndexBuffer *index_buffer) {
	return index_buffer->count;
}