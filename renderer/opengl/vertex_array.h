#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "renderer/opengl/buffers.h"
#include "renderer/opengl/renderer.h"

typedef struct OpenGLVertexArray OpenGLVertexArray;

OpenGLVertexArray *opengl_vertex_array_create(const OpenGLRenderer *opengl);
void opengl_vertex_array_destroy(OpenGLVertexArray *vertex_array);

void opengl_vertex_array_bind(const OpenGLVertexArray *vertex_array);
void opengl_vertex_array_unbind(const OpenGLVertexArray *vertex_array);

void opengl_vertex_array_add_vertex_buffer(OpenGLVertexArray *vertex_array, VertexBuffer *vertex_buffer);
void opengl_vertex_array_set_index_buffer(OpenGLVertexArray *vertex_array, IndexBuffer *index_buffer);

void opengl_vertex_array_draw(const OpenGLVertexArray *vertex_array);

const VertexBuffer *const *opengl_vertex_array_get_vertex_buffers(const OpenGLVertexArray *vertex_array, uint32 *count);
const IndexBuffer *opengl_vertex_array_get_index_buffer(const OpenGLVertexArray *vertex_array);

#endif // OPENGL_VERTEX_ARRAY_H