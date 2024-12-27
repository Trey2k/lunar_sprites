#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "renderer/opengl/buffers.h"
#include "renderer/opengl/renderer.h"

typedef struct OpenGLVertexArray OpenGLVertexArray;

OpenGLVertexArray *opengl_vertex_array_create(const OpenGLRenderer *opengl);
void opengl_vertex_array_destroy(OpenGLVertexArray *vertex_array);

void opengl_vertex_array_bind(const OpenGLVertexArray *vertex_array);
void opengl_vertex_array_unbind(const OpenGLVertexArray *vertex_array);

void opengl_vertex_array_set_vertex_buffers(OpenGLVertexArray *vertex_array, const VertexBuffer **vertex_buffers, size_t count);
void opengl_vertex_array_set_index_buffer(OpenGLVertexArray *vertex_array, const IndexBuffer *index_buffer);

void opengl_vertex_array_draw(const OpenGLVertexArray *vertex_array);
void opengl_vertex_array_draw_elements(const OpenGLVertexArray *vertex_array);

const IndexBuffer *opengl_vertex_array_get_index_buffer(const OpenGLVertexArray *vertex_array);

#endif // OPENGL_VERTEX_ARRAY_H