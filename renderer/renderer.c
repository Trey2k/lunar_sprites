#include "renderer/renderer.h"
#include "core/debug.h"
#include "core/memory.h"
#include "core/types/typedefs.h"

struct Renderer {
	const OS *os;

	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLRenderer *opengl_renderer;
#endif
	};
};

Renderer *renderer_create(RendererBackend backend, const OS *os) {
	LS_ASSERT(backend < RENDERER_BACKEND_COUNT && backend >= RENDERER_BACKEND_NONE);

	Renderer *renderer = ls_malloc(sizeof(Renderer));
	renderer->os = os;
	renderer->backend = backend;

	switch (backend) {
		case RENDERER_BACKEND_NONE: {
			return renderer;
		} break;
#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			renderer->opengl_renderer = opengl_renderer_create(os);
			LS_ASSERT(renderer->opengl_renderer);
			return renderer;
		} break;
#endif
		default:
			ls_log_fatal("Unknown renderer backend: %d\n", backend);
	}

	ls_free(renderer);

	return NULL;
}

void renderer_destroy(Renderer *renderer) {
	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;
		case RENDERER_BACKEND_OPENGL: {
#if defined(OPENGL_ENABLED)
			opengl_renderer_destroy(renderer->opengl_renderer);
#endif
		} break;
	}

	ls_free(renderer);
}

RendererBackend renderer_get_backend(const Renderer *renderer) {
	return renderer->backend;
}

#if defined(OPENGL_ENABLED)
const OpenGLRenderer *renderer_get_opengl(const Renderer *renderer) {
	return renderer->opengl_renderer;
}
#endif