#include "renderer/opengl//vertex_array.h"
#include "core/debug.h"
#include "renderer/buffers.h"
#include "renderer/opengl/buffers.h"
#include "renderer/opengl/debug.h"

#include <glad/gl.h>

static GLenum shader_data_type_to_opengl(ShaderDataType type) {
	switch (type) {
		case SHADER_DATA_TYPE_FLOAT:
		case SHADER_DATA_TYPE_FLOAT2:
		case SHADER_DATA_TYPE_FLOAT3:
		case SHADER_DATA_TYPE_FLOAT4:
		case SHADER_DATA_TYPE_MAT3:
		case SHADER_DATA_TYPE_MAT4:
			return GL_FLOAT;
		case SHADER_DATA_TYPE_INT:
		case SHADER_DATA_TYPE_INT2:
		case SHADER_DATA_TYPE_INT3:
		case SHADER_DATA_TYPE_INT4:
			return GL_INT;
		case SHADER_DATA_TYPE_BOOL:
			return GL_BOOL;

		default:
			LS_ASSERT_MSG(false, "Unkown shader data type: %d", type);
			return 0;
	}
};

struct OpenGLVertexArray {
	uint32 id;
	const OpenGLRenderer *opengl;

	uint32 vertex_buffer_count;
	VertexBuffer **vertex_buffers;
	IndexBuffer *index_buffer;
};

OpenGLVertexArray *opengl_vertex_array_create(const OpenGLRenderer *opengl) {
	OpenGLVertexArray *vertex_array = ls_malloc(sizeof(OpenGLVertexArray));
	vertex_array->opengl = (OpenGLRenderer *)opengl;

	vertex_array->vertex_buffer_count = 0;
	vertex_array->vertex_buffers = NULL;
	vertex_array->index_buffer = NULL;

	GL_CALL(glGenVertexArrays(1, &vertex_array->id));
	GL_CALL(glBindVertexArray(vertex_array->id));

	return vertex_array;
}

void opengl_vertex_array_destroy(OpenGLVertexArray *vertex_array) {
	for (uint32 i = 0; i < vertex_array->vertex_buffer_count; i++) {
		vertex_buffer_destroy(vertex_array->vertex_buffers[i]);
	}

	if (vertex_array->index_buffer) {
		index_buffer_destroy(vertex_array->index_buffer);
	}

	GL_CALL(glDeleteVertexArrays(1, &vertex_array->id));
	ls_free(vertex_array);
}

void opengl_vertex_array_bind(const OpenGLVertexArray *vertex_array) {
	GL_CALL(glBindVertexArray(vertex_array->id));
}

void opengl_vertex_array_unbind(const OpenGLVertexArray *vertex_array) {
	GL_CALL(glBindVertexArray(0));
}

void opengl_vertex_array_add_vertex_buffer(OpenGLVertexArray *vertex_array, VertexBuffer *vertex_buffer) {
	const BufferLayout *buffer_layout = vertex_buffer_get_layout(vertex_buffer);
	uint32 element_count = 0;
	const BufferElement *elements = buffer_layout_get_elements(buffer_layout, &element_count);
	LS_ASSERT(element_count > 0);

	opengl_vertex_array_bind(vertex_array);
	vertex_buffer_bind(vertex_buffer);

	uint32 index = 0;
	for (uint32 i = 0; i < element_count; i++) {
		const BufferElement *element = &elements[i];
		GL_CALL(glEnableVertexAttribArray(index));
		GL_CALL(
				glVertexAttribPointer(index,
						shader_data_type_get_component_count(element->type),
						shader_data_type_to_opengl(element->type),
						element->normalized ? GL_TRUE : GL_FALSE,
						buffer_layout_get_stride(buffer_layout),
						(const void *)(uintptr_t)element->offset));
		index++;
	}

	vertex_array->vertex_buffers = ls_realloc(vertex_array->vertex_buffers, sizeof(OpenGLVertexBuffer *) * (vertex_array->vertex_buffer_count + 1));
	vertex_array->vertex_buffers[vertex_array->vertex_buffer_count++] = vertex_buffer;
}

void opengl_vertex_array_set_index_buffer(OpenGLVertexArray *vertex_array, IndexBuffer *index_buffer) {
	opengl_vertex_array_bind(vertex_array);
	index_buffer_bind(index_buffer);

	vertex_array->index_buffer = index_buffer;
}

void opengl_vertex_array_draw(const OpenGLVertexArray *vertex_array) {
	opengl_vertex_array_bind(vertex_array);

	if (vertex_array->index_buffer) {
		GL_CALL(glDrawElements(GL_TRIANGLES, index_buffer_get_count(vertex_array->index_buffer), GL_UNSIGNED_INT, NULL));
	} else {
		uint32 count = 0;
		opengl_vertex_array_get_vertex_buffers(vertex_array, &count);
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, count));
	}
}

const VertexBuffer *const *opengl_vertex_array_get_vertex_buffers(const OpenGLVertexArray *vertex_array, uint32 *count) {
	*count = vertex_array->vertex_buffer_count;
	return (const VertexBuffer *const *)vertex_array->vertex_buffers;
}

const IndexBuffer *opengl_vertex_array_get_index_buffer(const OpenGLVertexArray *vertex_array) {
	return vertex_array->index_buffer;
}
