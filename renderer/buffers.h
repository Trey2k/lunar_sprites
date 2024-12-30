#ifndef BUFFERS_H
#define BUFFERS_H

#include "core/api.h"
#include "core/types/typedefs.h"
#include "renderer/renderer.h"

#include "core/core.h"

typedef enum {
	SHADER_DATA_TYPE_NONE = 0,
	SHADER_DATA_TYPE_FLOAT,
	SHADER_DATA_TYPE_FLOAT2,
	SHADER_DATA_TYPE_FLOAT3,
	SHADER_DATA_TYPE_FLOAT4,
	SHADER_DATA_TYPE_MAT3,
	SHADER_DATA_TYPE_MAT4,
	SHADER_DATA_TYPE_INT,
	SHADER_DATA_TYPE_INT2,
	SHADER_DATA_TYPE_INT3,
	SHADER_DATA_TYPE_INT4,
	SHADER_DATA_TYPE_BOOL
} ShaderDataType;

typedef enum {
	BUFFER_USAGE_STATIC = 0,
	BUFFER_USAGE_DYNAMIC,
	BUFFER_USAGE_STREAM
} BufferUsage;

_FORCE_INLINE_ uint32 shader_data_type_size(ShaderDataType type) {
	switch (type) {
		case SHADER_DATA_TYPE_FLOAT:
			return 4;
		case SHADER_DATA_TYPE_FLOAT2:
			return 4 * 2;
		case SHADER_DATA_TYPE_FLOAT3:
			return 4 * 3;
		case SHADER_DATA_TYPE_FLOAT4:
			return 4 * 4;
		case SHADER_DATA_TYPE_MAT3:
			return 4 * 3 * 3;
		case SHADER_DATA_TYPE_MAT4:
			return 4 * 4 * 4;
		case SHADER_DATA_TYPE_INT:
			return 4;
		case SHADER_DATA_TYPE_INT2:
			return 4 * 2;
		case SHADER_DATA_TYPE_INT3:
			return 4 * 3;
		case SHADER_DATA_TYPE_INT4:
			return 4 * 4;
		case SHADER_DATA_TYPE_BOOL:
			return 1;

		default:
			LS_ASSERT_MSG(false, "Unknown shader data type: %d", type);
			return 0;
	}
}

_FORCE_INLINE_ uint32 shader_data_type_get_component_count(ShaderDataType type) {
	switch (type) {
		case SHADER_DATA_TYPE_FLOAT:
			return 1;
		case SHADER_DATA_TYPE_FLOAT2:
			return 2;
		case SHADER_DATA_TYPE_FLOAT3:
			return 3;
		case SHADER_DATA_TYPE_FLOAT4:
			return 4;
		case SHADER_DATA_TYPE_MAT3:
			return 3 * 3;
		case SHADER_DATA_TYPE_MAT4:
			return 4 * 4;
		case SHADER_DATA_TYPE_INT:
			return 1;
		case SHADER_DATA_TYPE_INT2:
			return 2;
		case SHADER_DATA_TYPE_INT3:
			return 3;
		case SHADER_DATA_TYPE_INT4:
			return 4;
		case SHADER_DATA_TYPE_BOOL:
			return 1;

		default:
			LS_ASSERT_MSG(false, "Unknown shader data type: %d", type);
			return 0;
	}
}

typedef struct {
	ShaderDataType type;
	String name;
	uint32 offset;
	bool normalized;
} BufferElement;

typedef struct BufferLayout BufferLayout;

LS_EXPORT BufferLayout *buffer_layout_create(const BufferElement *elements, uint32 count);
LS_EXPORT void buffer_layout_destroy(BufferLayout *buffer_layout);

LS_EXPORT uint32 buffer_layout_get_stride(const BufferLayout *buffer_layout);
LS_EXPORT const BufferElement *buffer_layout_get_elements(const BufferLayout *buffer_layout, uint32 *count);

typedef struct VertexBuffer VertexBuffer;

LS_EXPORT VertexBuffer *renderer_create_vertex_buffer_empty(const Renderer *renderer, BufferUsage usage);
LS_EXPORT VertexBuffer *renderer_create_vertex_buffer(const Renderer *renderer, const void *data, uint32 size, BufferUsage usage);
LS_EXPORT void vertex_buffer_destroy(VertexBuffer *vertex_buffer);

LS_EXPORT void vertex_buffer_bind(const VertexBuffer *vertex_buffer);
LS_EXPORT void vertex_buffer_unbind(const VertexBuffer *vertex_buffer);
LS_EXPORT void vertex_buffer_set_data(VertexBuffer *vertex_buffer, const void *data, uint32 size);
LS_EXPORT void vertex_buffer_set_sub_data(VertexBuffer *vertex_buffer, const void *data, uint32 size, uint32 offset);
LS_EXPORT void vertex_buffer_set_layout(VertexBuffer *vertex_buffer, const BufferLayout *buffer_layout);
LS_EXPORT const BufferLayout *vertex_buffer_get_layout(const VertexBuffer *vertex_buffer);
LS_EXPORT uint32 vertex_buffer_get_count(const VertexBuffer *vertex_buffer);

typedef struct IndexBuffer IndexBuffer;

LS_EXPORT IndexBuffer *renderer_create_index_buffer_empty(const Renderer *renderer, BufferUsage usage);
LS_EXPORT IndexBuffer *renderer_create_index_buffer(const Renderer *renderer, const void *data, uint32 size, BufferUsage usage);
LS_EXPORT void index_buffer_destroy(IndexBuffer *index_buffer);

LS_EXPORT void index_buffer_bind(const IndexBuffer *index_buffer);
LS_EXPORT void index_buffer_unbind(const IndexBuffer *index_buffer);
LS_EXPORT void index_buffer_set_data(IndexBuffer *index_buffer, const void *data, uint32 size);
LS_EXPORT void index_buffer_set_sub_data(IndexBuffer *index_buffer, const void *data, uint32 size, uint32 offset);
LS_EXPORT uint32 index_buffer_get_count(const IndexBuffer *index_buffer);
#endif // BUFFERS_H