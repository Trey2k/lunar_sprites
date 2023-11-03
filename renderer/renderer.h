#ifndef RENDERER_H
#define RENDERER_H

#include "core/typedefs.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/opengl3_renderer.h"
#endif

#define RENDERER_BACKEND_COUNT 2

typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_OPENGL3,
} RendererBackend;

_FORCE_INLINE_ String renderer_backend_to_string(RendererBackend backend) {
	switch (backend) {
		case RENDERER_BACKEND_NONE:
			return "NONE";
		case RENDERER_BACKEND_OPENGL3:
			return "OPENGL3";
		default:
			return "UNKNOWN";
	}
}

typedef struct {
	RendererBackend backend;
	union {
#if defined(OPENGL3_ENABLED)
		OpenGL3Renderer *opengl3_renderer;
#endif
	};
} Renderer;

Renderer *renderer_create(RendererBackend backend);
void renderer_destroy(Renderer *renderer);

#endif