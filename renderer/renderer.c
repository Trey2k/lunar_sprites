#include "renderer/renderer.h"
#include "core/core.h"
#include "core/typedefs.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/opengl3_renderer.h"
#endif

Renderer *renderer_create(RendererBackend backend) {
	CORE_ASSERT(backend < RENDERER_BACKEND_COUNT - 1 && backend >= RENDERER_BACKEND_NONE);

	Renderer *renderer = core_malloc(sizeof(Renderer));
	renderer->backend = backend;

	switch (backend) {
		case RENDERER_BACKEND_NONE: {
		} break;
		case RENDERER_BACKEND_OPENGL3: {
#if defined(OPENGL3_ENABLED)
			renderer->opengl3_renderer = opengl3_renderer_create();
#endif
		} break;
	}

	return renderer;
}

void renderer_destroy(Renderer *renderer) {
	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;
		case RENDERER_BACKEND_OPENGL3: {
#if defined(OPENGL3_ENABLED)
			opengl3_renderer_destroy(renderer->opengl3_renderer);
#endif
		} break;
	}

	core_free(renderer);
}