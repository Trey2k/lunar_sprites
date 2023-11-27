#ifndef RENDERER_H
#define RENDERER_H

#include "core/core.h"

#include "renderer/typedefs.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_OPENGL,
} RendererBackend;

typedef struct Renderer Renderer;

Renderer *renderer_create(LSCore *core);
void renderer_start(Renderer *renderer);
void renderer_destroy(Renderer *renderer);

LSCore *renderer_get_core(const Renderer *renderer);

#if defined(OPENGL_ENABLED)
OpenGLRenderer *renderer_get_opengl(const Renderer *renderer);
#endif

// Sets the clear color for the renderer. There must be a valid rendering context.
LS_EXPORT void renderer_set_clear_color(const Renderer *renderer, float32 r, float32 g, float32 b, float32 a);
// Clears the screen with the clear color.
LS_EXPORT void renderer_clear(const Renderer *renderer);
// Returns the backend used by the renderer.
LS_EXPORT RendererBackend renderer_get_backend(const Renderer *renderer);

// Creates a vertex buffer. The data is expected to be compatible with the backend.
LS_EXPORT uint32 renderer_create_vertex_buffer(const Renderer *renderer, size_t size, void *data, UsageHint usage_hint);
// Destroys a vertex buffer.
LS_EXPORT void renderer_destroy_vertex_buffer(const Renderer *renderer, uint32 vertex_buffer);
// Binds the vertex buffer for use in the current rendering context.
LS_EXPORT void renderer_bind_vertex_buffer(const Renderer *renderer, TargetHint target, uint32 vertex_buffer);

// Creates a vertex array. The data is expected to be compatible with the backend.
LS_EXPORT uint32 renderer_create_vertex_array(const Renderer *renderer);
// Destroys a vertex array.
LS_EXPORT void renderer_destroy_vertex_array(const Renderer *renderer, uint32 vertex_array);
// Binds the vertex array for use in the current rendering context.
LS_EXPORT void renderer_bind_vertex_array(const Renderer *renderer, uint32 vertex_array);
// Sets a uniform in the vertex array. The vertex array must be bound before calling this function.
LS_EXPORT void renderer_set_vertex_array_uniform(const Renderer *renderer, uint32 vertex_array, int32 uniform_position, uint32 size, DataType data_type, bool normalized, size_t stride, size_t offset);

// Enables a vertex attribute. The vertex array must be bound before calling this function.
LS_EXPORT void renderer_enable_vertex_attrib_array(const Renderer *renderer, uint32 index);
// Draws the vertex array. The vertex array must be bound before calling this function.
LS_EXPORT void renderer_draw_arrays(const Renderer *renderer, DrawMode draw_mode, size_t first, size_t count);

_FORCE_INLINE_ String renderer_backend_to_string(RendererBackend backend) {
	switch (backend) {
		case RENDERER_BACKEND_NONE:
			return "NONE";
		case RENDERER_BACKEND_OPENGL:
			return "OPENGL3";
		default:
			return "UNKNOWN";
	}
}

#endif // RENDERER_H