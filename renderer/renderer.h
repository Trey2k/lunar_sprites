#ifndef RENDERER_H
#define RENDERER_H

#include "core/os/os.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

#if defined(GLES2_ENABLED)
#include "renderer/gles3/renderer.h"
#endif

#define RENDERER_BACKEND_COUNT 2
typedef enum {
	RENDERER_BACKEND_NONE,
	RENDERER_BACKEND_GLES2,
} RendererBackend;

typedef struct Renderer Renderer;

_FORCE_INLINE_ String renderer_backend_to_string(RendererBackend backend) {
	switch (backend) {
		case RENDERER_BACKEND_NONE:
			return "NONE";
		case RENDERER_BACKEND_GLES2:
			return "OPENGL3";
		default:
			return "UNKNOWN";
	}
}

Renderer *renderer_create(RendererBackend backend, const OS *os);
void renderer_destroy(Renderer *renderer);

RendererBackend renderer_get_backend(const Renderer *renderer);

#if defined(GLES2_ENABLED)
const GLES2Renderer *renderer_get_gles3(const Renderer *renderer);
#endif

#endif // RENDERER_H