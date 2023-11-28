#include "renderer/buffers.h"
#include "renderer/renderer.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/buffers.h"
#endif

// We cannot use if with the preprocessor, so we use this macro instead
#if defined(OPENGL_ENABLED)

#define GL_VERTEX_BUFFER_CALL(vertex_buffer, method, ...) \
	opengl_vertex_buffer_##method(vertex_buffer->opengl, ##__VA_ARGS__)

#define GL_INDEX_BUFFER_CALL(index_buffer, method, ...) \
	opengl_index_buffer_##method(index_buffer->opengl, ##__VA_ARGS__)

#else

#define GL_VERTEX_BUFFER_CALL(vertex_buffer, method, ...) \
	ls_log_fatal("OpenGL is not enabled\n");

#define GL_INDEX_BUFFER_CALL(index_buffer, method, ...) \
	ls_log_fatal("OpenGL is not enabled\n");

#endif // OPENGL_ENABLED

// VERTEX_BUFFER_CALL is for functions that do not return a value
#define VERTEX_BUFFER_CALL(vertex_buffer, method, ...)                              \
	switch (vertex_buffer->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                               \
		} break;                                                                    \
                                                                                    \
		case RENDERER_BACKEND_OPENGL: {                                             \
			GL_VERTEX_BUFFER_CALL(vertex_buffer, method, ##__VA_ARGS__);            \
		} break;                                                                    \
                                                                                    \
		default:                                                                    \
			ls_log_fatal("Unknown renderer backend: %d\n", vertex_buffer->backend); \
	}

// VERTEX_BUFFER_CALL_R is for functions that return a value
// The macro will only return if the call is successful
#define VERTEX_BUFFER_CALL_R(vertex_buffer, method, ...)                            \
	switch (vertex_buffer->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                               \
		} break;                                                                    \
                                                                                    \
		case RENDERER_BACKEND_OPENGL: {                                             \
			return GL_VERTEX_BUFFER_CALL(vertex_buffer, method, ##__VA_ARGS__);     \
		} break;                                                                    \
                                                                                    \
		default:                                                                    \
			ls_log_fatal("Unknown renderer backend: %d\n", vertex_buffer->backend); \
	}

// INDEX_BUFFER_CALL is for functions that do not return a value
#define INDEX_BUFFER_CALL(index_buffer, method, ...)                               \
	switch (index_buffer->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                              \
		} break;                                                                   \
                                                                                   \
		case RENDERER_BACKEND_OPENGL: {                                            \
			GL_INDEX_BUFFER_CALL(index_buffer, method, ##__VA_ARGS__);             \
		} break;                                                                   \
                                                                                   \
		default:                                                                   \
			ls_log_fatal("Unknown renderer backend: %d\n", index_buffer->backend); \
	};

// INDEX_BUFFER_CALL_R is for functions that return a value
// The macro will only return if the call is successful
#define INDEX_BUFFER_CALL_R(index_buffer, method, ...)                             \
	switch (index_buffer->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                              \
		} break;                                                                   \
                                                                                   \
		case RENDERER_BACKEND_OPENGL: {                                            \
			return GL_INDEX_BUFFER_CALL(index_buffer, method, ##__VA_ARGS__);      \
		} break;                                                                   \
                                                                                   \
		default:                                                                   \
			ls_log_fatal("Unknown renderer backend: %d\n", index_buffer->backend); \
	};

struct BufferLayout {
	BufferElement *elements;
	uint32 count;
	uint32 stride;
};

BufferLayout *buffer_layout_create(const BufferElement *elements, uint32 count) {
	BufferLayout *buffer_layout = ls_malloc(sizeof(BufferLayout));
	buffer_layout->elements = ls_malloc(sizeof(BufferElement) * count);
	buffer_layout->count = count;
	buffer_layout->stride = 0;

	uint32 offset = 0;
	for (uint32 i = 0; i < count; i++) {
		buffer_layout->elements[i] = elements[i];
		buffer_layout->elements[i].offset = offset;
		uint32 elm_size = shader_data_type_size(elements[i].type);
		buffer_layout->stride += elm_size;
		offset += elm_size;
	}

	return buffer_layout;
}

void buffer_layout_destroy(BufferLayout *buffer_layout) {
	ls_free(buffer_layout->elements);
	ls_free(buffer_layout);
}

uint32 buffer_layout_get_stride(const BufferLayout *buffer_layout) {
	return buffer_layout->stride;
}

const BufferElement *buffer_layout_get_elements(const BufferLayout *buffer_layout, uint32 *count) {
	*count = buffer_layout->count;
	return buffer_layout->elements;
}

struct VertexBuffer {
	RendererBackend backend;

	union {
#if defined(OPENGL_ENABLED)
		OpenGLVertexBuffer *opengl;
#endif
	};
};

VertexBuffer *renderer_create_vertex_buffer(const Renderer *renderer, const float32 *data, uint32 size) {
	VertexBuffer *vertex_buffer = ls_malloc(sizeof(VertexBuffer));
	vertex_buffer->backend = renderer_get_backend(renderer);

	switch (vertex_buffer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			vertex_buffer->opengl = opengl_create_vertex_buffer(data, size);
			if (!vertex_buffer->opengl) {
				ls_free(vertex_buffer);
				return NULL;
			}
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", vertex_buffer->backend);
	};

	return vertex_buffer;
}

void vertex_buffer_destroy(VertexBuffer *vertex_buffer) {
	VERTEX_BUFFER_CALL(vertex_buffer, destroy);
	ls_free(vertex_buffer);
}

void vertex_buffer_bind(const VertexBuffer *vertex_buffer) {
	VERTEX_BUFFER_CALL(vertex_buffer, bind);
}

void vertex_buffer_unbind(const VertexBuffer *vertex_buffer) {
	VERTEX_BUFFER_CALL(vertex_buffer, unbind);
}

void vertex_buffer_set_layout(VertexBuffer *vertex_buffer, const BufferLayout *buffer_layout) {
	VERTEX_BUFFER_CALL(vertex_buffer, set_layout, buffer_layout);
}

const BufferLayout *vertex_buffer_get_layout(const VertexBuffer *vertex_buffer) {
	VERTEX_BUFFER_CALL_R(vertex_buffer, get_layout);
	return NULL;
}

struct IndexBuffer {
	RendererBackend backend;

	union {
#if defined(OPENGL_ENABLED)
		OpenGLIndexBuffer *opengl;
#endif
	};
};

IndexBuffer *renderer_create_index_buffer(const Renderer *renderer, const uint32 *data, uint32 size) {
	IndexBuffer *index_buffer = ls_malloc(sizeof(IndexBuffer));
	index_buffer->backend = renderer_get_backend(renderer);

	switch (index_buffer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			index_buffer->opengl = opengl_index_buffer_create(data, size);
			if (!index_buffer->opengl) {
				ls_free(index_buffer);
				return NULL;
			}
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", index_buffer->backend);
	};

	return index_buffer;
}

void index_buffer_destroy(IndexBuffer *index_buffer) {
	INDEX_BUFFER_CALL(index_buffer, destroy);
	ls_free(index_buffer);
}

void index_buffer_bind(const IndexBuffer *index_buffer) {
	INDEX_BUFFER_CALL(index_buffer, bind);
}

void index_buffer_unbind(const IndexBuffer *index_buffer) {
	INDEX_BUFFER_CALL(index_buffer, unbind);
}

uint32 index_buffer_get_count(const IndexBuffer *index_buffer) {
	INDEX_BUFFER_CALL_R(index_buffer, get_count);
	return 0;
}