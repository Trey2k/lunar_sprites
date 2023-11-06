#include "renderer/context.h"
#include "core/core.h"
#include "core/typedefs.h"
#include "renderer/renderer.h"

#if defined(OPENGL3_ENABLED)
#include "renderer/opengl3/context.h"
#endif

Context *context_create(const LSWindow *window) {
	const RendererBackend backend = renderer_get_backend();

	Context *context = core_malloc(sizeof(Context));
	context->backend = backend;
	context->window = window;

	switch (backend) {
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			context->opengl3_context = opengl3_context_create(renderer_get_opengl3(), window);
			CORE_ASSERT(context->opengl3_context);
		} break;
#endif

		default:
			core_fatal("Unknown renderer backend: %d\n", backend);
	};

	return context;
}

void context_destroy(Context *context) {
	CORE_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			opengl3_context_destroy(context->opengl3_context);
		} break;
#endif

		default:
			core_fatal("Unknown renderer backend: %d\n", context->backend);
	};

	core_free(context);
}

void context_make_current(const Context *context) {
	CORE_ASSERT(context);

	switch (context->backend) {
#if defined(OPENGL3_ENABLED)
		case RENDERER_BACKEND_OPENGL3: {
			opengl3_context_make_current(context->opengl3_context);
		} break;
#endif

		default:
			core_fatal("Unknown renderer backend: %d\n", context->backend);
	};
}