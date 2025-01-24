#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "renderer/buffers.h"
#include "renderer/renderer.h"

typedef struct VertexArray VertexArray;

LS_EXPORT VertexArray *renderer_create_vertex_array();
LS_EXPORT void vertex_array_destroy(VertexArray *vertex_array);

LS_EXPORT void vertex_array_bind(const VertexArray *vertex_array);
LS_EXPORT void vertex_array_unbind(const VertexArray *vertex_array);
// Vertex buffer ownership is transferred to the vertex array if manage_buffers is true
LS_EXPORT void vertex_array_set_vertex_buffers(VertexArray *vertex_array, const VertexBuffer **vertex_buffer, size_t count);
// Index buffer ownership is transferred to the vertex array if manage_buffers is true
LS_EXPORT void vertex_array_set_index_buffer(VertexArray *vertex_array, const IndexBuffer *index_buffer);

LS_EXPORT void vertex_array_draw_elements(const VertexArray *vertex_array);
LS_EXPORT void vertex_array_draw(const VertexArray *vertex_array);

LS_EXPORT const IndexBuffer *vertex_array_get_index_buffer(const VertexArray *vertex_array);

#endif // VERTEX_ARRAY_H