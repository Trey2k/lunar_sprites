#ifndef OPENGL_BUFFERS_H
#define OPENGL_BUFFERS_H

#include "renderer/buffers.h"

#include "core/core.h"

typedef struct OpenGLVertexBuffer OpenGLVertexBuffer;

OpenGLVertexBuffer *opengl_create_vertex_buffer(const float32 *data, uint32 size);
void opengl_vertex_buffer_destroy(OpenGLVertexBuffer *vertex_buffer);

void opengl_vertex_buffer_bind(const OpenGLVertexBuffer *vertex_buffer);
void opengl_vertex_buffer_unbind(const OpenGLVertexBuffer *vertex_buffer);

void opengl_vertex_buffer_set_layout(OpenGLVertexBuffer *vertex_buffer, const BufferLayout *buffer_layout);
const BufferLayout *opengl_vertex_buffer_get_layout(const OpenGLVertexBuffer *vertex_buffer);

typedef struct OpenGLIndexBuffer OpenGLIndexBuffer;

OpenGLIndexBuffer *opengl_index_buffer_create(const uint32 *data, uint32 size);
void opengl_index_buffer_destroy(OpenGLIndexBuffer *index_buffer);

void opengl_index_buffer_bind(const OpenGLIndexBuffer *index_buffer);
void opengl_index_buffer_unbind(const OpenGLIndexBuffer *index_buffer);

uint32 opengl_index_buffer_get_count(const OpenGLIndexBuffer *index_buffer);

#endif // OPENGL_BUFFERS_H