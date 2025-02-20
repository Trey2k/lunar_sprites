#include "renderer/opengl//vertex_array.h"
#include "core/debug.h"
#include "core/types/typedefs.h"
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
			LS_ASSERT_MSG(false, "Unknown shader data type: %d", type);
			return 0;
	}
};

struct OpenGLVertexArray {
	uint32 id;
	const OpenGLRenderer *opengl;

	uint32 vertex_buffer_count;
	const IndexBuffer *index_buffer;
};

OpenGLVertexArray *opengl_vertex_array_create(const OpenGLRenderer *opengl) {
	OpenGLVertexArray *vertex_array = ls_malloc(sizeof(OpenGLVertexArray));
	vertex_array->opengl = (OpenGLRenderer *)opengl;

	vertex_array->vertex_buffer_count = 0;
	vertex_array->index_buffer = NULL;

	GL_CALL(glGenVertexArrays(1, &vertex_array->id));
	GL_CALL(glBindVertexArray(vertex_array->id));

	return vertex_array;
}

void opengl_vertex_array_destroy(OpenGLVertexArray *vertex_array) {
	GL_CALL(glDeleteVertexArrays(1, &vertex_array->id));
	ls_free(vertex_array);
}

void opengl_vertex_array_bind(const OpenGLVertexArray *vertex_array) {
	GL_CALL(glBindVertexArray(vertex_array->id));
}

void opengl_vertex_array_unbind(const OpenGLVertexArray *vertex_array) {
	GL_CALL(glBindVertexArray(0));
}

void opengl_vertex_array_set_vertex_buffers(OpenGLVertexArray *vertex_array, const VertexBuffer **vertex_buffers, size_t count) {
	vertex_array->vertex_buffer_count = 0;
	opengl_vertex_array_bind(vertex_array);

	uint32 index = 0;

	for (uint32 i = 0; i < count; i++) {
		const VertexBuffer *vertex_buffer = vertex_buffers[i];
		LS_ASSERT(vertex_buffer);

		const BufferLayout *buffer_layout = vertex_buffer_get_layout(vertex_buffer);
		uint32 element_count = 0;
		const BufferElement *elements = buffer_layout_get_elements(buffer_layout, &element_count);

		vertex_array->vertex_buffer_count += vertex_buffer_get_count(vertex_buffer);
		LS_ASSERT(element_count > 0);

		for (uint32 j = 0; j < element_count; j++) {
			const BufferElement *element = &elements[j];
			GL_CALL(glEnableVertexAttribArray(index));
			vertex_buffer_bind(vertex_buffer);
			GL_CALL(
					glVertexAttribPointer(index,
							shader_data_type_get_component_count(element->type),
							shader_data_type_to_opengl(element->type),
							element->normalized ? GL_TRUE : GL_FALSE,
							buffer_layout_get_stride(buffer_layout),
							(const void *)(uintptr_t)element->offset));
			index++;
		}
	}
}

void opengl_vertex_array_set_index_buffer(OpenGLVertexArray *vertex_array, const IndexBuffer *index_buffer) {
	opengl_vertex_array_bind(vertex_array);
	if (index_buffer) {
		index_buffer_bind(index_buffer);
	} else {
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	vertex_array->index_buffer = index_buffer;
}

void opengl_vertex_array_draw(const OpenGLVertexArray *vertex_array) {
	opengl_vertex_array_bind(vertex_array);
	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, vertex_array->vertex_buffer_count));
}

void opengl_vertex_array_draw_elements(const OpenGLVertexArray *vertex_array) {
	opengl_vertex_array_bind(vertex_array);
	LS_ASSERT_MSG(vertex_array->index_buffer, "%s", "No index buffer set for vertex array");
	GL_CALL(glDrawElements(GL_TRIANGLES, index_buffer_get_count(vertex_array->index_buffer), GL_UNSIGNED_INT, NULL));
}

const IndexBuffer *opengl_vertex_array_get_index_buffer(const OpenGLVertexArray *vertex_array) {
	return vertex_array->index_buffer;
}
