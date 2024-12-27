#include "renderer/vertex_array.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/vertex_array.h"
#endif

#if defined(OPENGL_ENABLED)
#define GL_VERTEX_ARRAY_CALL(vertex_array, method, ...) \
	opengl_vertex_array_##method(vertex_array->opengl, ##__VA_ARGS__)
#else
#define GL_VERTEX_ARRAY_CALL(vertex_array, method, ...) \
	ls_log_fatal("OpenGL is not enabled\n");
#endif // OPENGL_ENABLED

// VERTEX_ARRAY_CALL is for functions that do not return a value
#define VERTEX_ARRAY_CALL(vertex_array, method, ...)                               \
	switch (vertex_array->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                              \
		} break;                                                                   \
                                                                                   \
		case RENDERER_BACKEND_OPENGL: {                                            \
			GL_VERTEX_ARRAY_CALL(vertex_array, method, ##__VA_ARGS__);             \
		} break;                                                                   \
                                                                                   \
		default:                                                                   \
			ls_log_fatal("Unknown renderer backend: %d\n", vertex_array->backend); \
	}

// VERTEX_ARRAY_CALL_R is for functions that return a value
// The macro will only return if the call is successful
#define VERTEX_ARRAY_CALL_R(vertex_array, method, ...)                             \
	switch (vertex_array->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                              \
		} break;                                                                   \
                                                                                   \
		case RENDERER_BACKEND_OPENGL: {                                            \
			return GL_VERTEX_ARRAY_CALL(vertex_array, method, ##__VA_ARGS__);      \
		} break;                                                                   \
                                                                                   \
		default:                                                                   \
			ls_log_fatal("Unknown renderer backend: %d\n", vertex_array->backend); \
	}

struct VertexArray {
	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLVertexArray *opengl;
#endif
	};
};

VertexArray *renderer_create_vertex_array(const Renderer *renderer) {
	VertexArray *vertex_array = ls_malloc(sizeof(VertexArray));
	vertex_array->backend = renderer_get_backend(renderer);

	switch (vertex_array->backend) {
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			vertex_array->opengl = opengl_vertex_array_create(renderer_get_opengl(renderer));
			LS_ASSERT(vertex_array->opengl);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", vertex_array->backend);
	};

	return vertex_array;
}

void vertex_array_destroy(VertexArray *vertex_array) {
	VERTEX_ARRAY_CALL(vertex_array, destroy);
	ls_free(vertex_array);
}

void vertex_array_bind(const VertexArray *vertex_array) {
	VERTEX_ARRAY_CALL(vertex_array, bind);
}

void vertex_array_unbind(const VertexArray *vertex_array) {
	VERTEX_ARRAY_CALL(vertex_array, unbind);
}

void vertex_arrray_set_vertex_buffers(VertexArray *vertex_array, const VertexBuffer **vertex_buffers, size_t count) {
	VERTEX_ARRAY_CALL(vertex_array, set_vertex_buffers, vertex_buffers, count);
}

void vertex_array_set_index_buffer(VertexArray *vertex_array, const IndexBuffer *index_buffer) {
	VERTEX_ARRAY_CALL(vertex_array, set_index_buffer, index_buffer);
}

void vertex_array_draw(const VertexArray *vertex_array) {
	VERTEX_ARRAY_CALL(vertex_array, draw);
}

void vertex_array_draw_elements(const VertexArray *vertex_array) {
	VERTEX_ARRAY_CALL(vertex_array, draw_elements);
}

const IndexBuffer *vertex_array_get_index_buffer(const VertexArray *vertex_array) {
	VERTEX_ARRAY_CALL_R(vertex_array, get_index_buffer);
	return NULL;
}
