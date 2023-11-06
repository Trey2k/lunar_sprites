#include "renderer/renderer.h"
#include "core/core.h"
#include "core/typedefs.h"

struct Renderer {
	RendererBackend backend;
	union {
#if defined(OPENGL3_ENABLED)
		OpenGL3Renderer *opengl3_renderer;
#endif
	};
};

static struct Renderer renderer;

bool renderer_init(RendererBackend backend) {
	CORE_ASSERT(backend < RENDERER_BACKEND_COUNT && backend >= RENDERER_BACKEND_NONE);

	renderer.backend = backend;

	switch (backend) {
		case RENDERER_BACKEND_NONE: {
			return true;
		} break;
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			renderer.opengl3_renderer = opengl3_renderer_create();
			CORE_ASSERT(renderer.opengl3_renderer);
			return true;
		} break;
#endif
		default:
			core_fatal("Unknown renderer backend: %d\n", backend);
	}

	return false;
}

void renderer_deinit() {
	switch (renderer.backend) {
		case RENDERER_BACKEND_NONE: {
		} break;
		case RENDERER_BACKEND_OPENGL3: {
#if defined(OPENGL3_ENABLED)
			opengl3_renderer_destroy(renderer.opengl3_renderer);
#endif
		} break;
	}
}

RendererBackend renderer_get_backend() {
	return renderer.backend;
}

#if defined(OPENGL3_ENABLED)
const OpenGL3Renderer *renderer_get_opengl3() {
	return renderer.opengl3_renderer;
}
#endif