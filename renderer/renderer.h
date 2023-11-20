#ifndef RENDERER_H
#define RENDERER_H

#include "core/core.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

#define RENDERER_BACKEND_COUNT 2

typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_OPENGL,
} RendererBackend;

typedef struct Renderer Renderer;

Renderer *renderer_create(LSCore *core);
void renderer_start(Renderer *renderer);
void renderer_destroy(Renderer *renderer);

LS_EXPORT void renderer_set_clear_color(const Renderer *renderer, float32 r, float32 g, float32 b, float32 a);
LS_EXPORT void renderer_clear(const Renderer *renderer);

LS_EXPORT RendererBackend renderer_get_backend(const Renderer *renderer);

LSCore *renderer_get_core(const Renderer *renderer);

#if defined(OPENGL_ENABLED)
OpenGLRenderer *renderer_get_opengl(const Renderer *renderer);
#endif

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